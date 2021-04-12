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
    /** Node. */
    template<typename T>
    struct Node {
        T m_data;
        Node* m_prev;
        Node* m_next;
        Node(const T& data=T{}, Node* prev=nullptr, Node* next=nullptr)
            : m_data{data}, m_prev{prev}, m_next{next}
        { }
    }; // struct Node

    /** Iterator para listas de nodes. */
    template<typename T>
    class Iterator {
        friend list<T>;
        private:
            using node = Node<T>;

            /** Ponteiro para nó atual do iterador. */
            node* m_nodeptr;

        public:
            // Member types
            using difference_type   = std::ptrdiff_t;
            using value_type        = T;
            using reference         = T&;
            using iterator_category = std::bidirectional_iterator_tag;

            // Acessor 
            const node* get_nodeptr() const {
                return m_nodeptr;
            }

            // Construtores
            /** Construtor regular. */
            explicit Iterator(node* p = nullptr) 
                : m_nodeptr(p) { };
            /** Copy constructor */
            Iterator(const Iterator& other) {
                m_nodeptr = other.m_nodeptr; 
            }
            /** Assignment operator. */
            Iterator& operator=(const Iterator& other) {
                m_nodeptr = other.m_nodeptr;
                return *this; 
            }

            // Operadores unarios
            /** Operador de dereferencia. */
            reference operator*() const { 
                return m_nodeptr->m_data; 
            }
            /** Operacao de pre incremento `++it`. */
            Iterator& operator++() {
                m_nodeptr = m_nodeptr->m_next;
                return *this; 
            }
            /** Operacao de pos incremento `it++`. */
            Iterator operator++(int) {
                Iterator it_ret = *this;
                ++(*this);
                return it_ret;
            }
            /** Operacao de pre decremento `--it`. */
            Iterator& operator--() {
                m_nodeptr = m_nodeptr->m_prev;
                return *this; 
            }
            /** Operacao de pos decremento `it--`. */
            Iterator operator--(int) {
                Iterator it_ret{*this};
                --(*this);
                return it_ret;
            }

            // Operadores de comparacao
            /** Compara igualdade de iteradores comparando seus ponteiros.
             *
             * @returns `true` quando os ponteiros contidos pelos iteradores sao iguais e `false` caso contrario.
             * */
            bool operator==(const Iterator& other) const {
                return m_nodeptr == other.m_nodeptr; 
            }
            /** Compara desigualdade de iteradores comparando seus ponteiros.
             *
             * @returns `false` quando os ponteiros contidos pelos iteradores sao diferentes e `true` caso contrario.
             * */
            bool operator!=(const Iterator& other)const {
                return !(*this == other); 
            }

            // Funcoes friend
            /** Operacoes da forma e.g. `it + 14`. 
             *
             * @param   di  numero de "casas" a se deslocar a partir de `it`.
             * @returns novo Iterator apontando para a posicao nova.
             */
            friend Iterator operator+(const Iterator& it, difference_type di) {
                Iterator it_ret = it;
                while (di-- > 0) {
                    it_ret++;
                }
                return it_ret;
            }
            /** Operacoes da forma e.g. `it + 14`. 
             *
             * @param   di  numero de "casas" a se deslocar a partir de `it`.
             * @returns novo Iterator apontando para a posicao nova.
             */
            friend Iterator operator+(difference_type di, const Iterator& it) {
                return it + di; 
            }
            /** Operacoes da forma e.g. `it - 14`.
             *
             * @param di    quantidade de casas a deslocar na direcao reversa. 
             * @returns novo Iterator apontando para a posicao nova.
             */
            friend Iterator operator-(const Iterator& it, difference_type di) {
                Iterator it_ret = it;
                while (di-- > 0) {
                    it_ret--;
                }
                return it_ret;
            }
            /** Operacoes da forma e.g. `14 - it`.
             *
             * @param di    quantidade de casas a deslocar na direcao reversa. 
             * @returns novo Iterator apontando para a posicao nova.
             */
            friend Iterator operator-(difference_type di, const Iterator& it) {
                return it - di; 
            }
            /** Operacoes da forma e.g. `it1 - it2`.
             *
             * @returns "casas" de distancia entre os iteradores dados
             */
            friend difference_type operator-(Iterator last, Iterator first) { 
                difference_type ret = 0;
                while (first++ != last)
                    ret++;
                return ret;
            }

            node* nodeptr() {
                return m_nodeptr;
            }
    };  // class Iterator

    template<typename T>
    class list {
        public:
            using size_type  = size_t;
            using value_type = T;
            using reference  = T&;
            using node       = Node<T>;
            using iterator   = Iterator<T>;
            using citerator  = Iterator<const T>;

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
                return iterator{&m_tail};
            }
            citerator cbegin() const {
                return citerator{begin().get_nodeptr()};
            }
            citerator cend() const {
                return citerator{end().get_nodeptr()};
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

    }; // class list
} // namespace ls

#endif

