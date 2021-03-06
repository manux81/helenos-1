/*
 * Copyright (c) 2011 Petr Koupy
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup graph
 * @{
 */
/**
 * @file
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <as.h>
#include <stdlib.h>
#include "graph.h"

#define NAMESPACE         "graphemu"
#define VISUALIZER_NAME   "vsl"
#define RENDERER_NAME     "rnd"

static sysarg_t namespace_idx = 0;
static sysarg_t visualizer_idx = 0;
static sysarg_t renderer_idx = 0;

static LIST_INITIALIZE(visualizer_list);
static LIST_INITIALIZE(renderer_list);

static FIBRIL_MUTEX_INITIALIZE(visualizer_list_mtx);
static FIBRIL_MUTEX_INITIALIZE(renderer_list_mtx);

visualizer_t *graph_alloc_visualizer(void)
{
	return ((visualizer_t *) malloc(sizeof(visualizer_t)));
}

renderer_t *graph_alloc_renderer(void)
{
	// TODO
	return ((renderer_t *) malloc(sizeof(renderer_t)));
}

void graph_init_visualizer(visualizer_t *vs)
{
	link_initialize(&vs->link);
	atomic_flag_clear(&vs->claimed);
	vs->notif_sess = NULL;
	fibril_mutex_initialize(&vs->mode_mtx);
	list_initialize(&vs->modes);
	vs->mode_set = false;
	vs->cells.data = NULL;
	vs->dev_ctx = NULL;
}

void graph_init_renderer(renderer_t *rnd)
{
	// TODO
	link_initialize(&rnd->link);
	refcount_init(&rnd->ref_cnt);
}

errno_t graph_register_visualizer(visualizer_t *vs)
{
	char node[LOC_NAME_MAXLEN + 1];
	snprintf(node, LOC_NAME_MAXLEN, "%s%zu/%s%zu", NAMESPACE,
	    namespace_idx, VISUALIZER_NAME, visualizer_idx++);

	category_id_t cat;
	errno_t rc = loc_category_get_id("visualizer", &cat, 0);
	if (rc != EOK)
		return rc;

	rc = loc_service_register(node, &vs->reg_svc_handle);
	if (rc != EOK)
		return rc;

	rc = loc_service_add_to_cat(vs->reg_svc_handle, cat);
	if (rc != EOK) {
		loc_service_unregister(vs->reg_svc_handle);
		return rc;
	}

	fibril_mutex_lock(&visualizer_list_mtx);
	list_append(&vs->link, &visualizer_list);
	fibril_mutex_unlock(&visualizer_list_mtx);

	return rc;
}

errno_t graph_register_renderer(renderer_t *rnd)
{
	char node[LOC_NAME_MAXLEN + 1];
	snprintf(node, LOC_NAME_MAXLEN, "%s%zu/%s%zu", NAMESPACE,
	    namespace_idx, RENDERER_NAME, renderer_idx++);

	category_id_t cat;
	errno_t rc = loc_category_get_id("renderer", &cat, 0);
	if (rc != EOK)
		return rc;

	rc = loc_service_register(node, &rnd->reg_svc_handle);
	if (rc != EOK)
		return rc;

	rc = loc_service_add_to_cat(rnd->reg_svc_handle, cat);
	if (rc != EOK) {
		loc_service_unregister(rnd->reg_svc_handle);
		return rc;
	}

	fibril_mutex_lock(&renderer_list_mtx);
	list_append(&rnd->link, &renderer_list);
	fibril_mutex_unlock(&renderer_list_mtx);

	return rc;
}

visualizer_t *graph_get_visualizer(sysarg_t handle)
{
	visualizer_t *vs = NULL;

	fibril_mutex_lock(&visualizer_list_mtx);

	list_foreach(visualizer_list, link, visualizer_t, vcur) {
		if (vcur->reg_svc_handle == handle) {
			vs = vcur;
			break;
		}
	}

	fibril_mutex_unlock(&visualizer_list_mtx);

	return vs;
}

renderer_t *graph_get_renderer(sysarg_t handle)
{
	renderer_t *rnd = NULL;

	fibril_mutex_lock(&renderer_list_mtx);

	list_foreach(renderer_list, link, renderer_t, rcur) {
		if (rcur->reg_svc_handle == handle) {
			rnd = rcur;
			break;
		}
	}

	if (rnd)
		refcount_up(&rnd->ref_cnt);

	fibril_mutex_unlock(&renderer_list_mtx);

	return rnd;
}

errno_t graph_unregister_visualizer(visualizer_t *vs)
{
	fibril_mutex_lock(&visualizer_list_mtx);
	errno_t rc = loc_service_unregister(vs->reg_svc_handle);
	list_remove(&vs->link);
	fibril_mutex_unlock(&visualizer_list_mtx);

	return rc;
}

errno_t graph_unregister_renderer(renderer_t *rnd)
{
	fibril_mutex_lock(&renderer_list_mtx);
	errno_t rc = loc_service_unregister(rnd->reg_svc_handle);
	list_remove(&rnd->link);
	fibril_mutex_unlock(&renderer_list_mtx);

	return rc;
}

void graph_destroy_visualizer(visualizer_t *vs)
{
	assert(!atomic_flag_test_and_set(&vs->claimed));
	assert(vs->notif_sess == NULL);
	assert(!fibril_mutex_is_locked(&vs->mode_mtx));
	assert(list_empty(&vs->modes));
	assert(vs->mode_set == false);
	assert(vs->cells.data == NULL);
	assert(vs->dev_ctx == NULL);

	free(vs);
}

void graph_destroy_renderer(renderer_t *rnd)
{
	// TODO
	if (refcount_down(&rnd->ref_cnt))
		free(rnd);
}

errno_t graph_notify_mode_change(async_sess_t *sess, sysarg_t handle, sysarg_t mode_idx)
{
	async_exch_t *exch = async_exchange_begin(sess);
	errno_t ret = async_req_2_0(exch, VISUALIZER_MODE_CHANGE, handle, mode_idx);
	async_exchange_end(exch);

	return ret;
}

errno_t graph_notify_disconnect(async_sess_t *sess, sysarg_t handle)
{
	async_exch_t *exch = async_exchange_begin(sess);
	errno_t ret = async_req_1_0(exch, VISUALIZER_DISCONNECT, handle);
	async_exchange_end(exch);

	async_hangup(sess);

	return ret;
}

static void vs_claim(visualizer_t *vs, ipc_call_t *icall)
{
	vs->client_side_handle = ipc_get_arg1(icall);
	errno_t rc = vs->ops.claim(vs);
	async_answer_0(icall, rc);
}

static void vs_yield(visualizer_t *vs, ipc_call_t *icall)
{
	/* Deallocate resources for the current mode. */
	if (vs->mode_set) {
		if (vs->cells.data != NULL) {
			as_area_destroy((void *) vs->cells.data);
			vs->cells.data = NULL;
		}
	}

	/* Driver might also deallocate resources for the current mode. */
	errno_t rc = vs->ops.yield(vs);

	/*
	 * Now that the driver was given a chance to deallocate resources,
	 * current mode can be unset.
	 */
	if (vs->mode_set)
		vs->mode_set = false;

	async_answer_0(icall, rc);
}

static void vs_enumerate_modes(visualizer_t *vs, ipc_call_t *icall)
{
	size_t len;

	ipc_call_t call;
	if (!async_data_read_receive(&call, &len)) {
		async_answer_0(&call, EREFUSED);
		async_answer_0(icall, EREFUSED);
		return;
	}

	fibril_mutex_lock(&vs->mode_mtx);
	link_t *link = list_nth(&vs->modes, ipc_get_arg1(icall));

	if (link != NULL) {
		vslmode_list_element_t *mode_elem =
		    list_get_instance(link, vslmode_list_element_t, link);

		errno_t rc = async_data_read_finalize(&call, &mode_elem->mode, len);
		async_answer_0(icall, rc);
	} else {
		async_answer_0(&call, ENOENT);
		async_answer_0(icall, ENOENT);
	}

	fibril_mutex_unlock(&vs->mode_mtx);
}

static void vs_get_default_mode(visualizer_t *vs, ipc_call_t *icall)
{
	ipc_call_t call;
	size_t len;
	if (!async_data_read_receive(&call, &len)) {
		async_answer_0(&call, EREFUSED);
		async_answer_0(icall, EREFUSED);
		return;
	}

	fibril_mutex_lock(&vs->mode_mtx);
	vslmode_list_element_t *mode_elem = NULL;

	list_foreach(vs->modes, link, vslmode_list_element_t, cur) {
		if (cur->mode.index == vs->def_mode_idx) {
			mode_elem = cur;
			break;
		}
	}

	if (mode_elem != NULL) {
		errno_t rc = async_data_read_finalize(&call, &mode_elem->mode, len);
		async_answer_0(icall, rc);
	} else {
		fibril_mutex_unlock(&vs->mode_mtx);
		async_answer_0(&call, ENOENT);
		async_answer_0(icall, ENOENT);
	}

	fibril_mutex_unlock(&vs->mode_mtx);
}

static void vs_get_current_mode(visualizer_t *vs, ipc_call_t *icall)
{
	ipc_call_t call;
	size_t len;
	if (!async_data_read_receive(&call, &len)) {
		async_answer_0(&call, EREFUSED);
		async_answer_0(icall, EREFUSED);
		return;
	}

	if (vs->mode_set) {
		errno_t rc = async_data_read_finalize(&call, &vs->cur_mode, len);
		async_answer_0(icall, rc);
	} else {
		async_answer_0(&call, ENOENT);
		async_answer_0(icall, ENOENT);
	}
}

static void vs_get_mode(visualizer_t *vs, ipc_call_t *icall)
{
	ipc_call_t call;
	size_t len;
	if (!async_data_read_receive(&call, &len)) {
		async_answer_0(&call, EREFUSED);
		async_answer_0(icall, EREFUSED);
		return;
	}

	sysarg_t mode_idx = ipc_get_arg1(icall);

	fibril_mutex_lock(&vs->mode_mtx);
	vslmode_list_element_t *mode_elem = NULL;

	list_foreach(vs->modes, link, vslmode_list_element_t, cur) {
		if (cur->mode.index == mode_idx) {
			mode_elem = cur;
			break;
		}
	}

	if (mode_elem != NULL) {
		errno_t rc = async_data_read_finalize(&call, &mode_elem->mode, len);
		async_answer_0(icall, rc);
	} else {
		async_answer_0(&call, ENOENT);
		async_answer_0(icall, ENOENT);
	}

	fibril_mutex_unlock(&vs->mode_mtx);
}

static void vs_set_mode(visualizer_t *vs, ipc_call_t *icall)
{
	ipc_call_t call;
	size_t size;
	unsigned int flags;

	/* Retrieve the shared cell storage for the new mode. */
	if (!async_share_out_receive(&call, &size, &flags)) {
		async_answer_0(&call, EREFUSED);
		async_answer_0(icall, EREFUSED);
		return;
	}

	/* Retrieve mode index and version. */
	sysarg_t mode_idx = ipc_get_arg1(icall);
	sysarg_t mode_version = ipc_get_arg2(icall);

	/* Find mode in the list. */
	fibril_mutex_lock(&vs->mode_mtx);
	vslmode_list_element_t *mode_elem = NULL;

	list_foreach(vs->modes, link, vslmode_list_element_t, cur) {
		if (cur->mode.index == mode_idx) {
			mode_elem = cur;
			break;
		}
	}

	if (mode_elem == NULL) {
		fibril_mutex_unlock(&vs->mode_mtx);
		async_answer_0(&call, ENOENT);
		async_answer_0(icall, ENOENT);
		return;
	}

	/* Extract mode description from the list node. */
	vslmode_t new_mode = mode_elem->mode;
	fibril_mutex_unlock(&vs->mode_mtx);

	/* Check whether the mode is still up-to-date. */
	if (new_mode.version != mode_version) {
		async_answer_0(&call, EINVAL);
		async_answer_0(icall, EINVAL);
		return;
	}

	void *new_cell_storage;
	errno_t rc = async_share_out_finalize(&call, &new_cell_storage);
	if ((rc != EOK) || (new_cell_storage == AS_MAP_FAILED)) {
		async_answer_0(icall, ENOMEM);
		return;
	}

	/* Change device internal state. */
	rc = vs->ops.change_mode(vs, new_mode);

	/* Device driver could not establish new mode. Rollback. */
	if (rc != EOK) {
		as_area_destroy(new_cell_storage);
		async_answer_0(icall, ENOMEM);
		return;
	}

	/*
	 * Because resources for the new mode were successfully
	 * claimed, it is finally possible to free resources
	 * allocated for the old mode.
	 */
	if (vs->mode_set) {
		if (vs->cells.data != NULL) {
			as_area_destroy((void *) vs->cells.data);
			vs->cells.data = NULL;
		}
	}

	/* Insert new mode into the visualizer. */
	vs->cells.width = new_mode.screen_width;
	vs->cells.height = new_mode.screen_height;
	vs->cells.data = (pixel_t *) new_cell_storage;
	vs->cur_mode = new_mode;
	vs->mode_set = true;

	async_answer_0(icall, EOK);
}

static void vs_update_damaged_region(visualizer_t *vs, ipc_call_t *icall)
{
	sysarg_t x_offset = (ipc_get_arg5(icall) >> 16);
	sysarg_t y_offset = (ipc_get_arg5(icall) & 0x0000ffff);

	errno_t rc = vs->ops.handle_damage(vs,
	    ipc_get_arg1(icall), ipc_get_arg2(icall),
	    ipc_get_arg3(icall), ipc_get_arg4(icall),
	    x_offset, y_offset);
	async_answer_0(icall, rc);
}

static void vs_suspend(visualizer_t *vs, ipc_call_t *icall)
{
	errno_t rc = vs->ops.suspend(vs);
	async_answer_0(icall, rc);
}

static void vs_wakeup(visualizer_t *vs, ipc_call_t *icall)
{
	errno_t rc = vs->ops.wakeup(vs);
	async_answer_0(icall, rc);
}

void graph_visualizer_connection(visualizer_t *vs, ipc_call_t *icall, void *arg)
{
	/* Claim the visualizer. */
	if (atomic_flag_test_and_set(&vs->claimed)) {
		async_answer_0(icall, ELIMIT);
		return;
	}

	/* Accept the connection. */
	async_accept_0(icall);

	/* Establish callback session. */
	ipc_call_t call;
	async_get_call(&call);
	vs->notif_sess = async_callback_receive_start(EXCHANGE_SERIALIZE, &call);
	if (vs->notif_sess != NULL)
		async_answer_0(&call, EOK);
	else
		async_answer_0(&call, ELIMIT);

	/* Enter command loop. */
	while (true) {
		async_get_call(&call);

		if (!ipc_get_imethod(&call)) {
			async_answer_0(&call, EOK);
			break;
		}

		switch (ipc_get_imethod(&call)) {
		case VISUALIZER_CLAIM:
			vs_claim(vs, &call);
			break;
		case VISUALIZER_YIELD:
			vs_yield(vs, &call);
			goto terminate;
		case VISUALIZER_ENUMERATE_MODES:
			vs_enumerate_modes(vs, &call);
			break;
		case VISUALIZER_GET_DEFAULT_MODE:
			vs_get_default_mode(vs, &call);
			break;
		case VISUALIZER_GET_CURRENT_MODE:
			vs_get_current_mode(vs, &call);
			break;
		case VISUALIZER_GET_MODE:
			vs_get_mode(vs, &call);
			break;
		case VISUALIZER_SET_MODE:
			vs_set_mode(vs, &call);
			break;
		case VISUALIZER_UPDATE_DAMAGED_REGION:
			vs_update_damaged_region(vs, &call);
			break;
		case VISUALIZER_SUSPEND:
			vs_suspend(vs, &call);
			break;
		case VISUALIZER_WAKE_UP:
			vs_wakeup(vs, &call);
			break;
		default:
			async_answer_0(&call, EINVAL);
			goto terminate;
		}
	}

terminate:
	async_hangup(vs->notif_sess);
	vs->notif_sess = NULL;
	atomic_flag_clear(&vs->claimed);
}

void graph_renderer_connection(renderer_t *rnd, ipc_call_t *icall, void *arg)
{
	// TODO

	/* Accept the connection. */
	async_accept_0(icall);

	/* Enter command loop. */
	while (true) {
		ipc_call_t call;
		async_get_call(&call);

		if (!ipc_get_imethod(&call)) {
			async_answer_0(&call, EOK);
			break;
		}

		switch (ipc_get_imethod(&call)) {
		default:
			async_answer_0(&call, EINVAL);
			goto terminate;
		}
	}

terminate:
	graph_destroy_renderer(rnd);
}

void graph_client_connection(ipc_call_t *icall, void *arg)
{
	/* Find the visualizer or renderer with the given service ID. */
	visualizer_t *vs = graph_get_visualizer(ipc_get_arg2(icall));
	renderer_t *rnd = graph_get_renderer(ipc_get_arg2(icall));

	if (vs != NULL)
		graph_visualizer_connection(vs, icall, arg);
	else if (rnd != NULL)
		graph_renderer_connection(rnd, icall, arg);
	else
		async_answer_0(icall, ENOENT);
}

/** @}
 */
