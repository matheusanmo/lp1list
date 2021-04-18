#ifndef LIST_H
#define LIST_H

#include <iostream>  
using std::cout;
using std::endl;

#include <sstream>

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
                    friend const_iterator operator+(const_iterator it, difference_type di) {
                        const_iterator i = it;
                        while (di-- > 0)
                            ++i;
                        return i;
                    }
                    /** bwd jump */
                    friend const_iterator operator-(const_iterator it, difference_type di) {
                        const_iterator i = it;
                        while (di-- > 0)
                            --i;
                        return i;
                    }
                    /** difference */
                    friend difference_type operator-(const_iterator lhs, const_iterator rhs) {
                        difference_type d = 0;
                        while (lhs != rhs) {
                            d++;
                            rhs++;
                        }
                        return d;
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
                    /** referencia para o node apontado pelo iterador */
                    node& get_node() {
                        return *m_nodeptr;
                    }
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
                    /** posincremento */
                    iterator operator++(int) {
                        iterator old = *this;
                        ++(*this);
                        return old;
                    }
                    /** predecremento */
                    iterator& operator--() {
                        this->m_nodeptr = this->m_nodeptr->m_prev;
                        return *this;
                    }
                    /** posdecremento */
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
                    /** difference */
                    friend difference_type operator-(iterator lhs, iterator rhs) {
                        difference_type d = 0;
                        while (lhs != rhs) {
                            d++;
                            rhs++;
                        }
                        return d;
                    }
                    /** equality comparison */
                    bool operator==(iterator& rhs) const {
                        return this->m_nodeptr == rhs.m_nodeptr;
                    }
                    /** inequality comparison */
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
            /** Push front */
            void push_front(const T& val) {
                insert(begin(), val);
                return;
            }
            /** Push Back */
            void push_back(const T& val) {
                insert(end(), val);
                return;
            }
            /** Pop back */
            void pop_back() {
                erase(--end());
                return;
            }
            /** pop front */
            void pop_front() {
                erase(begin());
                return;
            }
            /** const front */
            const T& front() const {
                return *cbegin();
            }
            /** front */
            T& front() {
                return *begin();
            }
            /** const back */
            const T& back() const {
                return *(--cend());
            }
            /** back */
            T& back() {
                return *(--end());
            }
            /** equality comparison */
            bool operator==(const list& rhs) const {
                if (size() != rhs.size())
                    return false;
                const_iterator i_rhs = rhs.cbegin();
                for (const_iterator i_lhs = cbegin(); i_lhs != cend(); i_lhs++) {
                    if (*i_lhs != *i_rhs++)
                        return false;
                }
                return true;
            }
            /** inequality comparison */
            bool operator!=(const list& rhs) const {
                return !(*this == rhs);
            }
            /** range insert */
            template<typename InputIt>
            iterator insert(iterator pos, InputIt first, InputIt last) {
                while (first != last) {
                    pos = insert(pos, *first++);
                    pos++;
                }
                return --pos;
            }
            /** const range insert */
            template<typename InputIt>
            const_iterator insert(const_iterator pos, InputIt first, InputIt last) {
                while (first != last) {
                    pos = insert(pos, *first++);
                    pos++;
                }
                return --pos;
            }
            /** insert init list */
            iterator insert(iterator pos, std::initializer_list<T> ilist) {
                insert(pos, ilist.begin(), ilist.end());
                return pos;
            }
            /** const insert init list */
            const_iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
                insert(pos, ilist.begin(), ilist.end());
                return pos;
            }
            /** erase range */
            iterator erase(iterator first, iterator last) {
                iterator ret = first;
                while (first != last)
                    first = erase(first);
                return ret;
            }
            /** const erase range */
            const_iterator erase(const_iterator first, const_iterator last) {
                iterator ret = first;
                while (first != last)
                    first = erase(first);
                return ret;
            }
            /** to string */
            string to_string() const {
                std::ostringstream oss;
                oss << "<list m_size=" << m_size << ", data={ ";
                const_iterator i{};
                for (i = cbegin(); i+1 != cend(); i++) {
                    oss << *i << ", ";
                }
                oss << *i << " }>";
                return oss.str();
            }
            /** in place sort */
            void sort() {
                merge_sort(begin(), end());
            }
        private:
            /** in place merge sort no intervalo `[first, last)`. */
            void merge_sort(iterator first, iterator last) {
                if (last - first < 2)
                    return;
                iterator midpoint = first + ((last - first) / 2);
                merge_sort(first, midpoint);
                merge_sort(midpoint, last);
                merge_sort_merge(first, midpoint, last);
                return;
            }
            /** Recebe dois intervalos `[f_a, l_a)` e `[f_b, l_b)` e substitui um 
             * pelo outro em suas respectivas listas. Esta operacao eh O(1) pois apenas
             * "redireciona" os nodes adequadamente in place.
             *
             * @param   f_a iterator para primeiro elemendo da subslista `a`.
             * @param   l_a iterator para alem do ultimo elemento da sublista `a`.
             * @param   f_b iterator para primeiro elemento da sublista `b`.
             * @param   l_b iterator para alem do ultimo elemento da sublista `b`.
             */
            static void swap_sublists(iterator f_a, iterator l_a, iterator f_b, iterator l_b) {
                std::swap(f_a.get_node(), f_b.get_node());
                std::swap(l_a.get_node(), l_b.get_node());
                return;
            }

            /** merge sort worker */
            void merge_sort_merge(iterator first, iterator midpoint, iterator last) {
                list sorted{};
                iterator left = first;
                iterator right = midpoint;
                while (true) {
                    if (left == midpoint) {
                        sorted.insert(sorted.end(), right, last);
                        break; 
                    }
                    else if (right == last) {
                        sorted.insert(sorted.end(), left, midpoint);
                        break;
                    }
                    else if (*left < *right) {
                        sorted.push_back(*right++)
                    } else {
                        sorted.push_back(*left++);
                    }
                }
                swap_sublists(first, last, sorted.begin(), sorted.end());
                return;
            }
    }; // class list
} // namespace ls

#endif

