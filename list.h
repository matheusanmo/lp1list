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

namespace ls {
    /** Node. */
    template<typename T>
    struct Node {
        T m_data;
        Node* m_next;
        Node* m_prev;
        Node(const T& data=T{}, Node* next=nullptr, Node* prev=nullptr)
            : m_data{data}, m_next{next}, m_prev{prev}
        { }
    }; // struct Node

    /** Iterator para listas de nodes. */
    template<typename T>
    class Iterator {
        private:
            using node = Node<T>;

            /** Ponteiro para nó atual do iterador. */
            node* m_ptr;

        public:
            // Member types
            using difference_type   = std::ptrdiff_t;
            using value_type        = T;
            using reference         = T&;
            using iterator_category = std::bidirectional_iterator_tag;

            // Construtores
            /** Construtor regular. */
            Iterator(const node* p = nullptr) 
                : m_ptr(p) { };
            /** Copy constructor */
            Iterator(const Iterator& other) {
                m_ptr = other.m_ptr; 
            }
            /** Assignment operator. */
            Iterator& operator=(const Iterator& other) {
                m_ptr = other.m_ptr;
                return *this; 
            }

            // Operadores unarios
            /** Operador de dereferencia. */
            reference operator*() const { 
                return m_ptr->m_data; 
            }
            /** Operacao de pre incremento `++it`. */
            Iterator& operator++() {
                m_ptr = m_ptr->m_next;
                return *this; 
            }
            /** Operacao de pos incremento `it++`. */
            Iterator operator++(int) {
                Iterator it_ret = *this;
                (*this)++;
                return it_ret;
            }
            /** Operacao de pre decremento `--it`. */
            Iterator& operator--() {
                m_ptr = m_ptr->m_prev;
                return *this; 
            }
            /** Operacao de pos decremento `it--`. */
            Iterator operator--(int) {
                Iterator it_ret = *this;
                (*this)--;
                return it_ret;
            }

            // Operadores de comparacao
            /** Compara igualdade de iteradores comparando seus ponteiros.
             *
             * @returns `true` quando os ponteiros contidos pelos iteradores sao iguais e `false` caso contrario.
             * */
            bool operator==(const Iterator& other) const {
                return m_ptr == other.m_ptr; 
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
            friend difference_type operator-(const Iterator& it1, const Iterator& it2) { 
                difference_type ret = 0;
                Iterator it = it1;
                while (it++ != it2)
                    ret++;
                return ret;
            }

            node* nodeptr() {
                return m_ptr;
            }
    }; // class Iterator

    template<typename T>
    class list {
        public:
            using size_type  = size_t;
            using value_type = T;
            using node       = Node<T>;
            using iterator   = Iterator<T>;

        private:
            /** Tamanho da lista. */
            size_type m_size;
            /** Head node. */
            node m_head;
            /** Tail node. */
            node m_tail;

        public:
            /** Construtor regular. Constroi lista vazia. */
            list()
                : m_size{0}
                , m_head{node{value_type{}, &m_tail, nullptr}}
                , m_tail{node{value_type{}, nullptr, &m_head}}
            { }
            /** construtor que inicializa `count` elementos com `T{}` */
            list(size_type count) : list() {
                while (count > 0)
                    insert(begin(), T{});
            }
            /** range constructor */
            template<typename InputIt>
            list(InputIt first, InputIt last)
                : list()
            {
                while (first != last)
                    insert(end(), *first++);
            }
            /** copy constructor */
            list(const list& other) 
                : list(other.begin(), other.end())
            { }




            /** begin */
            iterator begin() {
                return iterator{m_head->m_next};
            }
            /** end */
            iterator end() {
                return iterator{&m_tail};
            }
            /** insert(pos, val) */
            iterator insert(iterator pos, const T& val) {
                node* node_new = new node{val, pos.nodeptr(), (pos-1).nodeptr()};
                pos->m_prev->m_next = node_new;
                pos->m_ptr = node_new;
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
            /** Destrutor. */
            ~list() {
                m_size = 0;
                node* i = &m_head;
                while (i != nullptr) {
                    node* next = i->m_next;
                    delete i;
                    i = next;
                }
            }
////            /** Acrescenta elemento no final da lista. */
 //           void push_back(const T& val) {
 //               node* node_new = new node{val, &m_tail, m_tail.prev};
 //               m_tail.m_prev->m_next = node_new;
 //               m_tail.m_prev = node_new;
 //               m_size++;
 //               return;
 //           }
    }; // class list
} // namespace ls

#endif

