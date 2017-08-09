// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.
#ifndef MYMAP_H
#define MYMAP_H
#include "support.h"
using namespace std;

template<typename KeyType, typename ValueType>
class MyMap
{
public:
    MyMap(){ m_size=0; m_root=nullptr;} //constructor
    ~MyMap(){deleteAll(m_root);} //destructor deletes all nodes
    void clear(){deleteAll(m_root); m_root=nullptr; m_size=0; } //deletes all nodes and resets variables
    int size() const {return m_size;} //returns number of associations

    //associates key with value, sometimes overwriting old value to do so
    void associate(const KeyType& key, const ValueType& value){associate(m_root, key,value);}

    //for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const{return find(m_root,key);}

    //for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

    //C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
    MyMap& operator=(const MyMap&) = delete;


private:

    //Node Struct
    struct Node{
        KeyType key;
        ValueType val;
        Node* left;
        Node* right;
        Node(const KeyType& key, const ValueType& val){
            this->key=key;
            this->val=val;
            left=nullptr;
            right=nullptr;
        }
    };

    //Variables
    Node* m_root;
    long long m_size;

    //Functions
    void deleteAll(Node* node){
        if(node==nullptr)return;
        deleteAll(node->right);
        deleteAll(node->left);
        delete node;
    }

    Node* associate(Node* node, const KeyType& key, const ValueType& value){
        if(node==nullptr){
          Node* newNode=new Node(key, value);
          m_size++;
          if(m_root==nullptr)m_root=newNode;
          return newNode;
        }
        if(node->key==key) node->val=value;
        else if(key < node->key) node->left=associate(node->left,key,value);
        else node->right=associate(node->right,key,value);
        return node;
    }

    const ValueType* find(Node* node, const KeyType& key) const{
        if(node==nullptr) return nullptr;
        if(node->key==key) return &node->val;
        else if(key < node->key )return find(node->left,key);
        else return find(node->right,key);
    }

};

#endif


