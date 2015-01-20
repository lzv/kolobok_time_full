#ifndef BINARYHEAP_H
#define BINARYHEAP_H

// Реализуем min-кучу. 

// Нода кучи будет хранить указатель на объект вместо самого объекта. Таким образом, при обмене местами двух нод, 
// можно будет обменять значения двух указателей на объекты и два веса, вместо десятка указателей - связей между нодами.

// ------------------------ 

template <typename T>
class BinaryHeap;

template <typename T>
class HeapNode;

template <typename T>
inline void SwapHeapNode (HeapNode <T> * first, HeapNode <T> * second);

template <typename T>
class HeapNode
{
private:
    T * _p_obj;
    int _weight;
    HeapNode <T> * _parent;
    HeapNode <T> * _first_child = nullptr;
    HeapNode <T> * _second_child = nullptr;
    
    HeapNode (T * p_obj, int weight, HeapNode <T> * parent)
        : _p_obj(p_obj), _weight(weight), _parent(parent)
    {}
    
    friend class BinaryHeap <T>;
    friend void SwapHeapNode <T> (HeapNode <T> *, HeapNode <T> *);
};

// ------------------------ 

template <typename T>
inline void SwapHeapNode (HeapNode <T> * first, HeapNode <T> * second)
{
    int wtemp = first->_weight;
    first->_weight = second->_weight;
    second->_weight = wtemp;
    T * ptemp = first->_p_obj;
    first->_p_obj = second->_p_obj;
    second->_p_obj = ptemp;
}

// ------------------------ 

template <typename T>
class BinaryHeap
{
public:
    BinaryHeap (bool delete_objects) : _delete_objects(delete_objects) {}
    
    ~BinaryHeap ()
    {
        clear();
    }
    
    void push (T * p_obj, int weight)
    {
        if (_next_insert_row == 0)
        {
            _root = new HeapNode <T> (p_obj, weight, nullptr);
            ++_next_insert_row;
            _max_count_for_current_row = 2;
        }
        else {
            auto parent = _root;
            unsigned long long int mask = 1ull << (_next_insert_row - 1);
            while (mask > 1ull)
            {
                parent = (_next_insert_pos & mask ? parent->_second_child : parent->_first_child);
                mask >>= 1;
            }
            auto new_node = new HeapNode <T> (p_obj, weight, parent);
            
            if (_next_insert_pos & 1ull) parent->_second_child = new_node;
            else parent->_first_child = new_node;
            
            ++_next_insert_pos;
            if (_next_insert_pos == _max_count_for_current_row)
            {
                ++_next_insert_row;
                _next_insert_pos = 0;
                _max_count_for_current_row <<= 1;
            }
            
            up_less_weight(new_node);
        }
    }
    
    void pop ()
    {
        if (_root != nullptr)
        {
            if (_next_insert_pos == 0)
            {
                --_next_insert_row;
                _max_count_for_current_row >>= 1;
                _next_insert_pos = _max_count_for_current_row - 1ull;
            }
            else {
                --_next_insert_pos;
            }
            
            if (_next_insert_row == 0)
            {
                if (_delete_objects) delete _root->_p_obj;
                delete _root;
                _root = nullptr;
            }
            else {
                unsigned long long int mask = 1ull << (_next_insert_row - 1);
                HeapNode <T> * last_node = _root;
                while (mask > 0ull)
                {
                    last_node = (_next_insert_pos & mask ? last_node->_second_child : last_node->_first_child);
                    mask >>= 1;
                }
                SwapHeapNode(_root, last_node);
                
                if (_next_insert_pos & 1ull) last_node->_parent->_second_child = nullptr;
                else last_node->_parent->_first_child = nullptr;
                
                if (_delete_objects) delete last_node->_p_obj;
                delete last_node;
                
                down_more_weight(_root);
            }
        }
    }
    
    // Поиск будет весьтись в ширину, так как будем искать объекты неподалеку от вершины кучи.
    void resort (T * p_obj, int delta_weight)
    {
        if (_root != nullptr)
        {
            if (_root->_p_obj == p_obj)
            {
                _root->_weight += delta_weight;
                if (delta_weight > 0) down_more_weight(_root);
            }
            else {
                // Есть проверенный список парентов. Проверяем их чаилдов, и по мере проверки заносим в массив. 
                // При проверке последней строки чаилдов в массив не заносим. 
                unsigned long long int max_count = _max_count_for_current_row >> 1;
                HeapNode <T> ** parents = new HeapNode <T> * [max_count];
                HeapNode <T> ** children = new HeapNode <T> * [max_count];
                unsigned int current_parents_row = 0;
                unsigned long long int max_count_in_current_parents_row = 1;
                parents[0] = _root;
                HeapNode <T> * result = nullptr;
                
                while (current_parents_row < _next_insert_row - 1)
                {
                    for (unsigned long long int i = 0ull; i < max_count_in_current_parents_row; ++i)
                    {
                        if (parents[i]->_first_child->_p_obj == p_obj)
                        {
                            result = parents[i]->_first_child;
                            current_parents_row = _next_insert_row;
                            break;
                        }
                        if (parents[i]->_second_child->_p_obj == p_obj)
                        {
                            result = parents[i]->_second_child;
                            current_parents_row = _next_insert_row;
                            break;
                        }
                        children[i * 2ull] = parents[i]->_first_child;
                        children[i * 2ull + 1ull] = parents[i]->_second_child;
                    }
                    auto temp = parents;
                    parents = children;
                    children = temp;
                    ++current_parents_row;
                    max_count_in_current_parents_row <<= 1;
                }
                
                if (result == nullptr)
                    for (unsigned long long int i = 0ull; i < max_count_in_current_parents_row; ++i)
                    {
                        if (parents[i]->_first_child == nullptr)
                        {
                            break;
                        }
                        else {
                            if (parents[i]->_first_child->_p_obj == p_obj)
                            {
                                result = parents[i]->_first_child;
                                break;
                            }
                        }
                        if (parents[i]->_second_child == nullptr)
                        {
                            break;
                        }
                        else {
                            if (parents[i]->_second_child->_p_obj == p_obj)
                            {
                                result = parents[i]->_second_child;
                                break;
                            }
                        }
                    }
                
                delete [] children;
                delete [] parents;
                
                if (result != nullptr)
                {
                    result->_weight += delta_weight;
                    if (delta_weight > 0) down_more_weight(result);
                    else if (delta_weight < 0) up_less_weight(result);
                }
            }
        }
    }
    
    // Рекурсивный поиск, глубина ограничивается по параметру old_weight. Заставляет дублировать вес вне кучи.
    // А так же, как ни странно, работает медленней resort (T * p_obj, int delta_weight).
    // Видимо, многократный вызов функций медленней выделения и удаления двух динамических массивов в памяти.
    /*void resort (T * p_obj, int old_weight, int delta_weight)
    {
        if (_root != nullptr)
        {
            auto result = find_recursively(_root, p_obj, old_weight);
            if (result != nullptr)
            {
                result->_weight += delta_weight;
                if (delta_weight > 0) down_more_weight(result);
                else if (delta_weight < 0) up_less_weight(result);
            }
        }
    }*/
    
    T * front ()
    {
        return _root->_p_obj;
    }
    
    bool empty ()
    {
        return _root == nullptr;
    }
    
    unsigned long long int size () 
    {
        return _max_count_for_current_row - 1 + _next_insert_pos;
    }
    
    void clear ()
    {
        if (_root != nullptr)
        {
            clear_node_recursively(_root);
            _root = nullptr;
        }
    }
    
private:
    BinaryHeap (const BinaryHeap &) = delete;
    BinaryHeap (const BinaryHeap &&) = delete;
    BinaryHeap & operator = (const BinaryHeap &) = delete;
    BinaryHeap & operator = (const BinaryHeap &&) = delete;
    
    HeapNode <T> * _root = nullptr;
    unsigned int _next_insert_row = 0;
    unsigned long long int _next_insert_pos = 0;
    unsigned long long int _max_count_for_current_row = 1;
    bool _delete_objects; // Удалять ли объекты, на которые указывает _p_obj, при удалении самой ноды.
    
    // Текущий элемент всплывает вверх, пока не займет соответствующее место. 
    void up_less_weight (HeapNode <T> * current)
    {
        if (current->_parent != nullptr && current->_weight < current->_parent->_weight)
        {
            SwapHeapNode(current, current->_parent);
            up_less_weight(current->_parent);
        }
    }
    
    // Текущий элемент опускается вниз, пока не займет соответствующее место. 
    void down_more_weight (HeapNode <T> * current)
    {
        if (current->_first_child != nullptr)
        {
            if (current->_second_child != nullptr)
            {
                HeapNode <T> * min_child = (current->_first_child->_weight < current->_second_child->_weight ? 
                                            current->_first_child : current->_second_child);
                if (min_child->_weight < current->_weight)
                {
                    SwapHeapNode(current, min_child);
                    down_more_weight(min_child);
                }
            }
            else {
                if (current->_first_child->_weight < current->_weight)
                    SwapHeapNode(current, current->_first_child);
            }
        }
    }
    
    // Рекурсивное удаление детей ноды и самой ноды. 
    void clear_node_recursively (HeapNode <T> * node)
    {
        if (node->_first_child != nullptr) clear_node_recursively(node->_first_child);
        if (node->_second_child != nullptr) clear_node_recursively(node->_second_child);
        if (_delete_objects) delete node->_p_obj;
        delete node;
    }
    
    // Рекурсивный поиск ноды по указателю на объект и весу. Слишком глубоко не опускаемся, так как есть ограничение weight. 
    /*HeapNode <T> * find_recursively (HeapNode <T> * current, T * p_obj, int weight)
    {
        if (current->_p_obj == p_obj) return current;
        HeapNode <T> * result = nullptr;
        if (current->_first_child != nullptr && current->_first_child->_weight <= weight) 
        {
            result = find_recursively(current->_first_child, p_obj, weight);
            if (result != nullptr) return result;
        }
        if (current->_second_child != nullptr && current->_second_child->_weight <= weight) 
            result = find_recursively(current->_second_child, p_obj, weight);
        return result;
    }*/
};

#endif // BINARYHEAP_H
