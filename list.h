#ifndef LIST_H
#define LIST_H

#include <iostream>  
using std::cout;
using std::endl;

#include <iterator>  // bidirectional_iterator_tag
#include <cassert>   // assert()
#include <algorithm> // copy
using std::copy;
#include <cstddef>   // std::ptrdiff_t
#include <type_traits>

#define DBG_PRINTLINE std::cout << __FILE__ << ":" << __LINE__ << std::endl

namespace sc {
    template<typename T>
    class list;

    template<typename T>
    class list {
        private:
            /** node. */
            struct node {
                T m_data;
                node* m_prev;
                node* m_next;
                node(const T& data=T{}, node* prev=nullptr, node* next=nullptr)
                    : m_data{data}, m_prev{prev}, m_next{next}
                { }
            }; // struct node

            /** Iterator para listas de nodes. */
            class const_iterator : public std::bidirectional_iterator_tag  {
                protected:
                    /** node apontado pelo iterador */
                    node* m_nodeptr;
                    /* Construtor a partir de ponteiro para node */
                    const_iterator(node* p=nullptr) 
                        : m_nodeptr(p) { }
                    friend class list;

                public:
                    /** construtor vazio */
                    const_iterator() 
                        : m_nodeptr(nullptr) { }
                    /** dereferencia */
                    const T& operator* () const {
                        return m_nodeptr->m_data;
                    }
                    /** copy ctor */
                    const_iterator(const const_iterator&) = default;
                    /** dtor */
                    ~const_iterator() = default;
                    /** preincremento */
                    const_iterator& operator++() {
                        m_nodeptr = m_nodeptr->m_next;
                        return *this;
                    }
                    /** posincremento */
                    const_iterator operator++(int) {
                        const_iterator i_old(*this);
                        ++(*this);
                        return i_old;
                    }
                    /** predecremento */
                    const_iterator& operator--() {
                        m_nodeptr = m_nodeptr->m_prev;
                        return *this;
                    }
                    /** posdecremento */
                    const_iterator operator--(int) {
                        const_iterator i_old(*this);
                        --(*this);
                        return i_old;
                    }
                    bool operator==(const const_iterator& rhs) const {
                        return m_nodeptr == rhs.m_nodeptr;
                    }
                    bool operator!=(const const_iterator& rhs) const {
                        return m_nodeptr != rhs.m_nodeptr;
                }
            }; //class const_iterator

            /** iterador nao constante */
            class iterator : public const_iterator {
                public:
                    T& operator*() { return this->m_nodeptr; }
                    iterator(node* nodeptr=nullptr) : const_iterator(nodeptr) { }

                protected:
                    friend class list;
            }; // class iterator

        public:
            using size_type  = size_t;
            using value_type = T;
            using reference  = T&;

        private:
            /** Tamanho da lista. */
            size_type m_size;
            /** Head node. */
            node m_head;
            /** Tail node. */
            node m_tail;

        public:
            //
            const node& head() {
                return m_head;
            }
            const node& tail() {
                return m_tail;
            }
            /** Construtor regular. Constroi lista vazia. */
            list()
                : m_size{0}
                , m_head{node{value_type{}, nullptr, &m_tail}}
                , m_tail{node{value_type{}, &m_head, nullptr}}
            { }
            /** destrutor */
            ~list() {
                clear();
            }
            /** size */
            size_type size() const {
                return m_size;
            }
            /** empty */
            bool empty() const {
                return size() == 0;
            }
            /** begin */
            iterator begin() {
                return iterator(m_head.m_next);
            }
            /** end */
            iterator end() {
                return iterator(&m_tail);
            }
            const_iterator cbegin() {
                return const_iterator(m_head.m_next);
            }
            const_iterator cend() {
                return const_iterator(&m_tail);
            }
            /** insert(pos, val) */
            iterator insert(iterator pos, const T& val) {
                node* node_new = new node(val, (pos-1).m_nodeptr, pos.m_nodeptr);
                (pos-1).m_nodeptr->m_next = node_new;
                pos.m_nodeptr->m_prev = node_new;
                m_size++;
                return pos - 1;
            }
            /** erase pos, retorna ponteiro para quem seria pos+1 */
            iterator erase(iterator pos) {
                node* node_deleted = pos.nodeptr();
                node* node_next    = (pos+1).nodeptr();
                node* node_prev    = (pos-1).nodeptr();
                node_next->m_prev = node_prev;
                node_prev->m_next = node_next;
                delete node_deleted;
                m_size--;
                return iterator{node_next};
            }
            /** clear */
            void clear() {
                while (begin() != end())
                    erase(begin());
            }
            /** size constructor */
            list(size_type size) : list() {
                while (size-- > 0) 
                    insert(begin(), T{});
            }
            /** range constructor */
            template<typename InputIt>
            list(InputIt first, InputIt last) : list(){
                while (first != last)
                    insert(end(), *first++);
            }
            /** ilist constructor */
            list(std::initializer_list<T> ilist)
                : list(ilist.begin(), ilist.end()) 
            { }
            /** copy constructor */
            list(const list& other) : list() { }

    }; // class list

    template<class InputIt>
    InputIt next(InputIt it, typename InputIt::difference_type n = 1) {
        return it + n;
    }

} // namespace ls

#endif

