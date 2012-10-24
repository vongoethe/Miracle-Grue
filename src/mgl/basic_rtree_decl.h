/* 
 * File:   basic_rtree_decl.h
 * Author: Dev
 *
 * Created on October 1, 2012, 10:00 AM
 */

#ifndef MGL_BASIC_RTREE_DECL_H
#define	MGL_BASIC_RTREE_DECL_H

#include "mgl.h"
#include "spacial_data.h"
#include <sstream>
#include <memory>
#include <utility>
//#include "FSBAllocator.h"

namespace mgl {

template <typename T, size_t C>
class basic_rtree_intersect_comparator;

class TreeException : public Exception {
public:
    template <typename T>
    TreeException(const T& arg) : Exception(arg) {}
};

static const size_t RTREE_DEFAULT_BRANCH = 4;

template <typename T, size_t C = RTREE_DEFAULT_BRANCH>
class basic_rtree {
public:
    typedef T value_type;
    
    static value_type* DEFAULT_DATA_PTR() { return NULL; }
    static basic_rtree* DEFAULT_CHILD_PTR() { return NULL; }
    typedef std::allocator<basic_rtree> tree_alloc_t;
    //typedef FSB::FSBAllocator<basic_rtree> tree_alloc_t;
    typedef typename tree_alloc_t::template rebind<value_type>::other value_alloc_t;
    
    class iterator{
    public:
        iterator() {}
        template <typename U>
        iterator(const U&) {  }
    };
    typedef iterator const_iterator;
    
    basic_rtree();
    basic_rtree(const basic_rtree& other);
    basic_rtree& operator =(const basic_rtree& other);
    ~basic_rtree();
    
    iterator insert(const value_type& value);
    void erase(iterator iter);
    template <typename COLLECTION, typename FILTER>
    void search(COLLECTION& result, const FILTER& filt);
    
    iterator begin() { return iterator(); }
    iterator end() { return iterator(); }
    
    void repr(std::ostream& out, size_t recursionLevel = 0);
    void repr_svg(std::ostream& out, size_t recursionLevel = 0);
    
private:
    

    template <typename U, size_t V> 
    friend class basic_rtree_intersect_comparator;
    
    explicit basic_rtree(const value_type& value);
    basic_rtree(bool canReproduce);
    
    static const size_t CAPACITY = C;
    
    typedef std::pair<size_t, size_t> child_index_pair;
    typedef typename std::pair<basic_rtree*, basic_rtree*> child_ptr_pair;
    
    void insert(basic_rtree* child);
    void insertDumb(basic_rtree* child);
    void unlinkChild(size_t childIndex);
    void insertIntelligently(basic_rtree* child);   //here is rtree logic
    void split(basic_rtree* child);   //clone child, distribute its children
    
    child_index_pair pick_furthest_children() const;
    void distributeChildPair(child_ptr_pair childs, child_ptr_pair to);
    void distributeChild(basic_rtree* child, child_ptr_pair to);
    
    void adopt(basic_rtree* from);
    void growTree();
    void regrowBounds();
    
    bool isLeaf() const { return myData; }
    bool isFull() const { return size() >= capacity(); }
    bool isEmpty() const { return !isLeaf() && !hasChildren(); }
    bool hasChildren() const { return size(); }
    bool needSplitting() const { return isFull(); }
    size_t size() const { return myChildrenCount; }
    size_t capacity() const { return CAPACITY; }
    
    bool splitMyself;   //true for root
    
    AABBox myBounds;
    basic_rtree* myChildren[CAPACITY];
    size_t myChildrenCount;
    
    value_type* myData;
    
    tree_alloc_t myTreeAllocator;
    value_alloc_t myDataAllocator;
};

template <typename T, size_t C>
class basic_rtree_intersect_comparator {
    typedef basic_rtree<T, C> tree_type;
public:
    basic_rtree_intersect_comparator(tree_type* b) : base(b) {}
    bool operator ()(const tree_type* a, const tree_type* b) const {
        Scalar origPerimeter = base->myBounds.perimeter();
        return base->myBounds.expandedTo(a->myBounds).perimeter() - origPerimeter < 
                base->myBounds.expandedTo(b->myBounds).perimeter() - origPerimeter;
    }
private:
    tree_type* base;
};



}

#endif	/* BASIC_RTREE_DECL_H */
