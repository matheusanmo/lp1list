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
        public:
            using size_type  = size_t;
            using value_type = T;
            using reference  = T&;
            using difference_type = std::ptrdiff_t;

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
            /** Tamanho da lista. */
            size_type m_size;
            /** Head node. */
            node* m_head;
            /** Tail node. */
            node* m_tail;

        public:
            /** Iterator para listas de nodes. */
            class const_iterator : public std::bidirectional_iterator_tag  {
                public:
                    typedef T              value_type;
                    typedef std::ptrdiff_t difference_type;
                    typedef T&             reference;
                    typedef T*             pointer;

                protected:
                    /** node apontado pelo iterador */
                    node* m_nodeptr;
                    /* Construtor a partir de ponteiro para node */
                    const_iterator(node* p) 
                        : m_nodeptr(p) { }
                    friend class list;

                public:
                    using iterator_category = std::bidirectional_iterator_tag;
                    /** Ctor vazio */
                    const_iterator() : m_nodeptr(nullptr) { }
                    /** copy ctor */
                    const_iterator(const const_iterator&) = default;
                    /** dtor */
                    ~const_iterator() = default;
                    /** dereferencia */
                    const T& operator*() const {
                        return m_nodeptr->m_data;
                    }
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
                    /** fwd jump */
                    friend const_iterator operator+(const_iterator& it, difference_type di) {
                        const_iterator i = it;
                        while (di-- > 0)
                            ++i;
                        return i;
                    }
                    /** bwd jump */
                    friend const_iterator operator-(const_iterator& it, difference_type di) {
                        const_iterator i = it;
                        while (di-- > 0)
                            --i;
                        return i;
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
                    typedef T              value_type;
                    typedef std::ptrdiff_t difference_type;
                    typedef T&             reference;
                    typedef T*             pointer;

                protected:
                    iterator(node* p) : const_iterator(p) { }
                    friend class list;

                public:
                    using iterator_category = std::bidirectional_iterator_tag;
                    /** empty ctor */
                    /** dereferencia const */
                    const T& operator*() const {
                        return this->m_nodeptr->m_data;
                    }
                    iterator() : const_iterator() { }
                    /** derefencia nao const */
                    T& operator*() {
                        return this->m_nodeptr->m_data;
                    }
                    /** preincremento */
                    iterator& operator++() {
                        this->m_nodeptr = this->m_nodeptr->m_next;
                        return *this;
                    }
                    iterator operator++(int) {
                        iterator old = *this;
                        ++(*this);
                        return old;
                    }
                    iterator& operator--() {
                        this->m_nodeptr = this->m_nodeptr->m_prev;
                        return *this;
                    }
                    iterator operator--(int) {
                        iterator old = *this;
                        --(*this);
                        return old;
                    }
                    /** fwd jump */
                    friend iterator operator+(const iterator& it, difference_type di) {
                        iterator i = it;
                        while (di-- > 0)
                            ++i;
                        return i;
                    }
                    /** bwd jump */
                    friend iterator operator-(iterator& it, difference_type di) {
                        iterator i = it;
                        while (di-- > 0)
                            --i;
                        return i;
                    }
                    bool operator==(iterator& rhs) const {
                        return this->m_nodeptr == rhs.m_nodeptr;
                    }
                    bool operator!=(const iterator& rhs) const {
                        return this->m_nodeptr != rhs.m_nodeptr;
                }
            }; // class iterator

        public:
            /** Construtor regular. Constroi lista vazia. */
            list() {
                m_size = 0;
                m_head = new node(value_type{}, nullptr, nullptr);
                m_tail = new node(value_type{}, m_head, nullptr);
                m_head->m_next = m_tail;
            }
            /** destrutor */
            ~list() {
                clear();
                delete m_head;
                delete m_tail;
            }
            /** size constructor */
            explicit list(size_type size) : list() {
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
            list(list& other) : list(other.begin(), other.end()) { }
            /** assignment operator */
            list& operator=(const list& other) {
                clear();
                const_iterator it = other.cbegin();
                while (it != other.cend())
                    insert(cend(), *it++);
                return *this;
            }
            /** clear */
            void clear() {
                while (begin() != end())
                    erase(begin());
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
                return iterator(m_head->m_next);
            }
            /** end */
            iterator end() {
                return iterator(m_tail);
            }
            /** cbegin */
            const_iterator cbegin() const {
                return const_iterator(m_head->m_next);
            }
            /** cend */
            const_iterator cend() const {
                return const_iterator(m_tail);
            }
            /** insert(pos, val) */
            const_iterator insert(const_iterator pos, const T& val) {
                node* node_new = new node(val, (pos-1).m_nodeptr, pos.m_nodeptr);
                (pos-1).m_nodeptr->m_next = node_new;
                pos.m_nodeptr->m_prev = node_new;
                m_size++;
                return pos - 1;
            }
            /** erase pos, retorna ponteiro para quem seria pos+1 */
            const_iterator erase(const_iterator pos) {
                node* node_deleted = pos.m_nodeptr;
                node* node_next    = (pos+1).m_nodeptr;
                node* node_prev    = (pos-1).m_nodeptr;
                node_next->m_prev = node_prev;
                node_prev->m_next = node_next;
                delete node_deleted;
                m_size--;
                return const_iterator{node_next};
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
                node* node_deleted = pos.m_nodeptr;
                node* node_next    = (pos+1).m_nodeptr;
                node* node_prev    = (pos-1).m_nodeptr;
                node_next->m_prev = node_prev;
                node_prev->m_next = node_next;
                delete node_deleted;
                m_size--;
                return iterator{node_next};
            }
    }; // class list
} // namespace ls

#endif

