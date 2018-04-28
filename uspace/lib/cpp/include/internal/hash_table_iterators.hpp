/*
 * Copyright (c) 2018 Jaroslav Jindrak
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

#ifndef LIBCPP_INTERNAL_HASH_TABLE_ITERATORS
#define LIBCPP_INTERNAL_HASH_TABLE_ITERATORS

#include <internal/list.hpp>
#include <internal/hash_table_bucket.hpp>
#include <iterator>

namespace std::aux
{
    template<class Value, class ConstReference, class ConstPointer, class Size>
    class hash_table_const_iterator
    {
        public:
            using value_type      = Value;
            using size_type       = Size;
            using const_reference = ConstReference;
            using const_pointer   = ConstPointer;
            using difference_type = ptrdiff_t;

            using iterator_category = forward_iterator_tag;

            hash_table_const_iterator(const hash_table_bucket<value_type, size_type>* table = nullptr,
                                      size_type idx = size_type{}, size_type max_idx = size_type{},
                                      const list_node<value_type>* current = nullptr)
                : table_{table}, idx_{idx}, max_idx_{max_idx}, current_{current}
            { /* DUMMY BODY */ }

            hash_table_const_iterator(const hash_table_const_iterator&) = default;
            hash_table_const_iterator& operator=(const hash_table_const_iterator&) = default;

            const_reference operator*() const
            {
                return current_->value;
            }

            const_pointer operator->() const
            {
                return &current_->value;
            }

            hash_table_const_iterator& operator++()
            {
                current_ = current_->next;
                if (current_ == table_[idx_].head)
                {
                    if (idx_ < max_idx_)
                    {
                        while (!table_[++idx_].head && idx_ < max_idx_)
                        { /* DUMMY BODY */ }

                        if (idx_ < max_idx_)
                            current_ = table_[idx_].head;
                        else
                            current_ = nullptr;
                    }
                    else
                        current_ = nullptr;
                }

                return *this;
            }

            hash_table_const_iterator operator++(int)
            {
                auto tmp_current = current_;
                auto tmp_idx = idx_;

                current_ = current_->next;
                if (current_ == table_[idx_].head)
                {
                    if (idx_ < max_idx_)
                    {
                        while (!table_[++idx_].head && idx_ < max_idx_)
                        { /* DUMMY BODY */ }

                        if (idx_ < max_idx_)
                            current_ = table_[idx_].head;
                        else
                            current_ = nullptr;
                    }
                    else
                        current_ = nullptr;
                }

                return hash_table_const_iterator{
                    table_, tmp_idx, max_idx_, tmp_current
                };
            }

            list_node<value_type>* node()
            {
                return const_cast<list_node<value_type>*>(current_);
            }

            const list_node<value_type>* node() const
            {
                return current_;
            }

            size_type idx() const
            {
                return idx_;
            }

        private:
            const hash_table_bucket<value_type, size_type>* table_;
            size_type idx_;
            size_type max_idx_;
            const list_node<value_type>* current_;
    };

    template<class Value, class ConstRef, class ConstPtr, class Size>
    bool operator==(const hash_table_const_iterator<Value, ConstRef, ConstPtr, Size>& lhs,
                    const hash_table_const_iterator<Value, ConstRef, ConstPtr, Size>& rhs)
    {
        return lhs.node() == rhs.node();
    }

    template<class Value, class ConstRef, class ConstPtr, class Size>
    bool operator!=(const hash_table_const_iterator<Value, ConstRef, ConstPtr, Size>& lhs,
                    const hash_table_const_iterator<Value, ConstRef, ConstPtr, Size>& rhs)
    {
        return !(lhs == rhs);
    }

    template<class Value, class Reference, class Pointer, class Size>
    class hash_table_iterator
    {
        public:
            using value_type      = Value;
            using size_type       = Size;
            using reference       = Reference;
            using pointer         = Pointer;
            using difference_type = ptrdiff_t;

            using iterator_category = forward_iterator_tag;

            hash_table_iterator(hash_table_bucket<value_type, size_type>* table = nullptr,
                                size_type idx = size_type{}, size_type max_idx = size_type{},
                                list_node<value_type>* current = nullptr)
                : table_{table}, idx_{idx}, max_idx_{max_idx}, current_{current}
            { /* DUMMY BODY */ }

            hash_table_iterator(const hash_table_iterator&) = default;
            hash_table_iterator& operator=(const hash_table_iterator&) = default;

            reference operator*()
            {
                return current_->value;
            }

            pointer operator->()
            {
                return &current_->value;
            }

            hash_table_iterator& operator++()
            {
                current_ = current_->next;
                if (current_ == table_[idx_].head)
                {
                    if (idx_ < max_idx_)
                    {
                        while (!table_[++idx_].head && idx_ < max_idx_)
                        { /* DUMMY BODY */ }

                        if (idx_ < max_idx_)
                            current_ = table_[idx_].head;
                        else
                            current_ = nullptr;
                    }
                    else
                        current_ = nullptr;
                }

                return *this;
            }

            hash_table_iterator operator++(int)
            {
                auto tmp_current = current_;
                auto tmp_idx = idx_;

                current_ = current_->next;
                if (current_ == table_[idx_].head)
                {
                    if (idx_ < max_idx_)
                    {
                        while (!table_[++idx_].head && idx_ < max_idx_)
                        { /* DUMMY BODY */ }

                        if (idx_ < max_idx_)
                            current_ = table_[idx_].head;
                        else
                            current_ = nullptr;
                    }
                    else
                        current_ = nullptr;
                }

                return hash_table_iterator{
                    table_, tmp_idx, max_idx_, tmp_current
                };
            }

            list_node<value_type>* node()
            {
                return current_;
            }

            const list_node<value_type>* node() const
            {
                return current_;
            }

            size_type idx() const
            {
                return idx_;
            }

            template<class ConstRef, class ConstPtr>
            operator hash_table_const_iterator<
                Value, ConstRef, ConstPtr, Size
            >() const
            {
                return hash_table_const_iterator<value_type, ConstRef, ConstPtr, size_type>{
                    table_, idx_, max_idx_, current_
                };
            }

        private:
            hash_table_bucket<value_type, size_type>* table_;
            size_type idx_;
            size_type max_idx_;
            list_node<value_type>* current_;
    };

    template<class Value, class Ref, class Ptr, class Size>
    bool operator==(const hash_table_iterator<Value, Ref, Ptr, Size>& lhs,
                    const hash_table_iterator<Value, Ref, Ptr, Size>& rhs)
    {
        return lhs.node() == rhs.node();
    }

    template<class Value, class Ref, class Ptr, class Size>
    bool operator!=(const hash_table_iterator<Value, Ref, Ptr, Size>& lhs,
                    const hash_table_iterator<Value, Ref, Ptr, Size>& rhs)
    {
        return !(lhs == rhs);
    }

    template<class Value, class ConstReference, class ConstPointer>
    class hash_table_const_local_iterator
    {
        public:
            using value_type      = Value;
            using const_reference = ConstReference;
            using const_pointer   = ConstPointer;
            using difference_type = ptrdiff_t;

            using iterator_category = forward_iterator_tag;

            // TODO: requirement for forward iterator is default constructibility, fix others!
            hash_table_const_local_iterator(const list_node<value_type>* head = nullptr,
                                            const list_node<value_type>* current = nullptr)
                : head_{head}, current_{current}
            { /* DUMMY BODY */ }

            hash_table_const_local_iterator(const hash_table_const_local_iterator&) = default;
            hash_table_const_local_iterator& operator=(const hash_table_const_local_iterator&) = default;

            const_reference operator*() const
            {
                return current_->value;
            }

            const_pointer operator->() const
            {
                return &current_->value;
            }

            hash_table_const_local_iterator& operator++()
            {
                current_ = current_->next;
                if (current_ == head_)
                    current_ = nullptr;

                return *this;
            }

            hash_table_const_local_iterator operator++(int)
            {
                auto tmp = current_;
                current_ = current_->next;
                if (current_ == head_)
                    current_ = nullptr;

                return hash_table_const_local_iterator{head_, tmp};
            }


            list_node<value_type>* node()
            {
                return const_cast<list_node<value_type>*>(current_);
            }

            const list_node<value_type>* node() const
            {
                return current_;
            }

        private:
            const list_node<value_type>* head_;
            const list_node<value_type>* current_;
    };

    template<class Value, class ConstRef, class ConstPtr>
    bool operator==(const hash_table_const_local_iterator<Value, ConstRef, ConstPtr>& lhs,
                    const hash_table_const_local_iterator<Value, ConstRef, ConstPtr>& rhs)
    {
        return lhs.node() == rhs.node();
    }

    template<class Value, class ConstRef, class ConstPtr>
    bool operator!=(const hash_table_const_local_iterator<Value, ConstRef, ConstPtr>& lhs,
                    const hash_table_const_local_iterator<Value, ConstRef, ConstPtr>& rhs)
    {
        return !(lhs == rhs);
    }

    template<class Value, class Reference, class Pointer>
    class hash_table_local_iterator
    {
        public:
            using value_type      = Value;
            using reference       = Reference;
            using pointer         = Pointer;
            using difference_type = ptrdiff_t;

            using iterator_category = forward_iterator_tag;

            hash_table_local_iterator(list_node<value_type>* head = nullptr,
                                      list_node<value_type>* current = nullptr)
                : head_{head}, current_{current}
            { /* DUMMY BODY */ }

            hash_table_local_iterator(const hash_table_local_iterator&) = default;
            hash_table_local_iterator& operator=(const hash_table_local_iterator&) = default;

            reference operator*()
            {
                return current_->value;
            }

            pointer operator->()
            {
                return &current_->value;
            }

            hash_table_local_iterator& operator++()
            {
                current_ = current_->next;
                if (current_ == head_)
                    current_ = nullptr;

                return *this;
            }

            hash_table_local_iterator operator++(int)
            {
                auto tmp = current_;
                current_ = current_->next;
                if (current_ == head_)
                    current_ = nullptr;

                return hash_table_local_iterator{head_, tmp};
            }

            list_node<value_type>* node()
            {
                return current_;
            }

            const list_node<value_type>* node() const
            {
                return current_;
            }

            template<class ConstRef, class ConstPtr>
            operator hash_table_const_local_iterator<
                Value, ConstRef, ConstPtr
            >() const
            {
                return hash_table_const_local_iterator<
                    value_type, ConstRef, ConstPtr
                >{head_, current_};
            }

        private:
            list_node<value_type>* head_;
            list_node<value_type>* current_;
    };

    template<class Value, class Ref, class Ptr>
    bool operator==(const hash_table_local_iterator<Value, Ref, Ptr>& lhs,
                    const hash_table_local_iterator<Value, Ref, Ptr>& rhs)
    {
        return lhs.node() == rhs.node();
    }

    template<class Value, class Ref, class Ptr>
    bool operator!=(const hash_table_local_iterator<Value, Ref, Ptr>& lhs,
                    const hash_table_local_iterator<Value, Ref, Ptr>& rhs)
    {
        return !(lhs == rhs);
    }
}

#endif
