// Project identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the 'priority queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure to
            // initialize it here.
            explicit Node(const TYPE &val)
            : elt{ val }, child{ nullptr }, sibling{ nullptr }, previous{nullptr}
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            // TODO: Add one extra pointer (parent or previous) as desired.
            Node *previous;
        
    }; // Node


    // Description: Construct an empty pairing heap with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        // TODO: Implement this function.
            root=nullptr;
            numNodes=0;
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
            root=nullptr;
            numNodes=0;
            while(start!=end){
                push(*start);
                start++;
            }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare } {
        // TODO: Implement this function.
        // NOTE: The structure does not have to be identical to the original,
        //       but it must still be a valid Pairing Heap.
            
            std::deque<Node*> dq;
            Node* temp;
            root=nullptr;
            numNodes=0;
            if(other.root){
                dq.push_back(other.root);
            }
            while(dq.size()!=0){
                temp=dq.front();
                if(temp->sibling){
                    dq.push_back(temp->sibling);
                }
                if(temp->child){
                    dq.push_back(temp->child);
                }
                push(dq.front()->elt);
                dq.pop_front();
                temp=nullptr;
            }
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ &operator=(const PairingPQ &rhs) {
        // TODO: Implement this function.
        // HINT: Use the copy-swap method from the "Arrays and Containers" lecture.
        PairingPQ temp(rhs);
        std::swap(numNodes,temp.numNodes);
        std::swap(root,temp.root);
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        if(root==nullptr){
            return;
        }
        std::deque<Node*> dq;
        Node* temp=root;
        dq.push_back(temp);
        while(!dq.empty()){
            temp=dq.front();
            if(temp->sibling!=nullptr){
                dq.push_back(temp->sibling);
            }
            if(temp->child!=nullptr){
                dq.push_back(temp->child);
            }
            //dq.pop_front();
            //temp->sibling=nullptr;
            //temp->child=nullptr;
            //temp->previous=nullptr;
            delete temp;
            //std::cout<<"dq: "<<dq.front()->elt<<std::endl;
            dq.pop_front();
            //std::cout<<"whole tree: "<<dq.empty()<<std::endl;
            temp=nullptr;
        }
        numNodes=0;
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out of order and
    //              'rebuilds' the pairing heap by fixing the pairing heap invariant.
    //              You CANNOT delete 'old' nodes and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        if(!root||!root->child){
            return;
        }
        Node* newRoot=nullptr;
        std::deque<Node*> dq;
        Node* temp;
        temp=root;
        dq.push_back(temp);
        while(!dq.empty()){
            temp=dq.front();
            if(temp->sibling){
                dq.push_back(temp->sibling);
            }
            if(temp->child){
                dq.push_back(temp->child);
            }
            temp->sibling=nullptr;
            temp->child=nullptr;
            temp->previous=nullptr;
            if(newRoot==nullptr){
                newRoot=temp;
                dq.pop_front();
                temp=nullptr;
                continue;
            }
            newRoot=meld(newRoot,temp);
            dq.pop_front();
            //std::cout<<"dq.empty: "<<dq.empty()<<std::endl;
            temp=nullptr;
        }
        root=newRoot;
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already done.
    //              You should implement push functionality entirely in the addNode()
    //              function, and this function calls addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the pairing heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the pairing heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        std::deque<Node*> temp;
        
        Node* pt=root->child;
        if(!pt){
            delete root;
            root=nullptr;
            numNodes--;
            return;
        }
        Node* n1;
        Node* n2;
        while(pt!=nullptr){
            temp.push_back(pt);
            pt=pt->sibling;
        }
        while(temp.size()>1){
            n1=temp.front();
            n1->sibling=nullptr;
            n1->previous=nullptr;
            temp.pop_front();
            n2=temp.front();
            n2->sibling=nullptr;
            n2->previous=nullptr;
            temp.pop_front();
            temp.push_back(meld(n1,n2));
        }
        root->child=nullptr;
        delete root;
        root=temp.front();
        numNodes--;
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function
        return root->elt;
        
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function
        return numNodes;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function
        return numNodes==0;
    } // empty()


    // Description: Updates the priority of an element already in the pairing heap by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    // TODO: when you implement this function, uncomment the parameter names.
    void updateElt(Node* node, const TYPE &new_value) {
        // TODO: Implement this function
        node->elt=new_value;
        if(node->previous){
            if(node->previous->child!=node){
                node->previous->sibling=node->sibling;
            } else{
                node->previous->child=node->sibling;
            }
            if(node->sibling){
                node->sibling->previous=node->previous;
            }
            node->sibling=nullptr;
            node->previous=nullptr;
            root=meld(root,node);
        } else{
            root=node;
        }
    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE &val) {
        // TODO: Implement this function
        Node* nowpush=new Node(val);
        if(root==nullptr){
            root=nowpush;
        } else{
            root=meld(root,nowpush);
        }
        numNodes++;
        return nowpush;
    } // addNode()


private:
    // TODO: Add any additional member variables or member functions you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).
    //vedio: 32"
    Node* meld(Node* ptr1,Node* ptr2){ //no previous, no sibling
        if(this->compare(ptr1->elt,ptr2->elt)){
            if(ptr2->child==nullptr){
                ptr2->child=ptr1;
                ptr1->previous=ptr2;
                //ptr1->sibling=nullptr;
            } else{
                ptr1->sibling=ptr2->child;
                ptr2->child->previous=ptr1;
                ptr1->previous=ptr2;
                ptr2->child=ptr1;
            }
            return ptr2;
        } else{
            if(ptr1->child==nullptr){
                ptr1->child=ptr2;
                ptr2->previous=ptr1;
            } else{
                ptr2->sibling=ptr1->child;
                ptr1->child->previous=ptr2;
                ptr2->previous=ptr1;
                ptr1->child=ptr2;
            }
            return ptr1;
        }
    }
    
    Node* root;
    uint32_t numNodes;

    // NOTE: For member variables, you are only allowed to add a "root pointer"
    //       and a "count" of the number of nodes.  Anything else (such as a deque)
    //       should be declared inside of member functions as needed.
};


#endif // PAIRINGPQ_H
