#include "chain.h"
#include "chain_given.cpp"

// PA1 functions

/**
 * Destroys the current Chain. This function should ensure that
 * memory does not leak on destruction of a chain.
 */
Chain::~Chain(){
  clear();
}

/**
 * Inserts a new node at the end of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
void Chain::insertBack(const Block & ndata){
  Node *extra = new Node(ndata);
  extra->prev = head_->prev;
  head_->prev->next = extra;
  head_->prev = extra;
  extra->next = head_;
  length_+=1;

  width_ = ndata.width();
  height_ = ndata.height();
}

/**
 * Modifies the Chain by moving the subchain of len Nodes,
 * starting at position startPos, dist positions toward the
 * end of the chain.  If startPos + len - 1 + dist > length
 * then dist = length - startPos - len + 1 (to prevent gaps
 * in the new chain).
 * The subchain occupies positions (startPos + dist) through
 * (startPos + dist + len - 1) of the resulting chain.
 * The order of subchain nodes is not changed in the move.
 * You may assume that: 1 <= startPos <= length - len + 1,
 * 0 <= dist <= length, and 0 <= len <= length. 
 */
void Chain::moveBack(int startPos, int len, int dist){

  if (dist == 0) {
  } else {
 
  Node * first = walk(head_,startPos);
    Node * end = walk(head_,startPos+len-1);
    Node * endAfter = walk(head_,startPos+len);
    Node * firstPrev = walk(head_,startPos-1);
    Node * newPrev = walk(head_,startPos+dist+len-1);

    if (endAfter == head_->prev)
        return;

    firstPrev->next = endAfter;
    endAfter->prev = firstPrev;
    end->next = newPrev->next;
    (newPrev->next)->prev = end;
    newPrev->next=  first;
    first->prev = newPrev;
  }
}

/**
 * Rolls the current Chain by k nodes: reorders the current list
 * so that the first k nodes are the (n-k+1)th, (n-k+2)th, ... , nth
 * nodes of the original list followed by the 1st, 2nd, ..., (n-k)th
 * nodes of the original list where n is the length.
 */
void Chain::roll(int k){
  if (head_->next != head_) {
    Node * oneafterhead = walk(head_, 1);

    Node * first = walk(head_, length_-k+1);
    Node * last = walk(first, k-1);

    Node * beforefirst = walk(head_, length_-k);
    Node * afterlast = walk(first, k);

    if (k != length_) {
      if (first->prev != head_) {
        
        first->prev = head_;
        head_->next = first;

        last->next = oneafterhead;
        oneafterhead->prev = last;

        beforefirst->next = afterlast;
        afterlast->prev = beforefirst;
      }
    }
  }
}

/**
 * Modifies the current chain by reversing the sequence
 * of nodes from pos1 up to and including pos2. You may
 * assume that pos1 and pos2 exist in the given chain,
 * and pos1 <= pos2.
 * The positions are 1-based.
 */
void Chain::reverseSub(int pos1, int pos2){
  if (head_->next != head_) {
    Node * current = walk(head_, pos1);

    if (pos1 != pos2) {
      Node * fpos = walk(head_, pos1);
      Node * second = walk(head_, pos1+1);
      Node * pos2p = walk(head_, pos2);
      Node * obf = walk(head_, pos1-1);  
      Node * oas = walk(head_, pos2+1);

      while (current != pos2p) {
        Node * secondnext = second->next;
        current->prev = second;
        second->next = current;

        current = second;
        second = secondnext;
      }

      obf->next = pos2p;
      pos2p->prev = obf;

      oas->prev = fpos;
      fpos->next = oas;
    }
  }
}

/*
* Modifies both the current chain and the "other" chain by removing
* nodes from the other chain and adding them between the nodes
* of the current chain -- one "other" node between two current nodes --
* until one of the two chains is exhausted.  Then the rest of the
* nodes in the non-empty chain follow.
* The length of the resulting chain should be the sum of the lengths
* of current and other. The other chain should have only
* the head_ sentinel at the end of the operation.
* The weave fails and the original
* chains should be unchanged if block sizes are different.
* In that case, the result of the function should be:
* cout << "Block sizes differ." << endl;
*/
void Chain::weave(Chain & other) { // leaves other empty.
if (other.size() == 0) {return;}

if (height_ != other.height_ || width_ != other.width_) {
    cout <<"Block sizes differ." << endl;
    return;
  }

  Node * curr1 = head_->next;
  while (curr1 != head_) {

   Node * curr2 = other.getNext();



    if (curr2 == NULL) {
      return;
    }

    Node * n = curr1->next;
    
    curr1->next = curr2;
    curr2->prev = curr1;
    curr2->next = n;
    n->prev = curr2;

    curr1 = n;

    length_++;
  }

  if (other.length_ > 0){

    Node * first = other.head_->next;
    Node * end = other.head_->prev;
    Node * l = head_->prev;

    l->next = first;
    first->prev = l;
    head_->prev = end;
    end->next = head_;

    length_ = length_ + other.length_;
    other.length_ = 0;

    other.head_->next = other.head_;
    other.head_->prev = other.head_;
  }
}


/**
 * Destroys all dynamically allocated memory associated with the
 * current Chain class except for the sentinel head_. Sets length_
 * to zero.  After clear() the chain represents an empty chain.
 */
void Chain::clear() {
  Node * curr = head_->next;
  while (curr != head_) {
    Node * a = curr->next;
    curr->next = NULL;
    curr->prev = NULL;
    delete curr;
    curr = a;
  }
  head_->next = head_;
  head_->prev = head_;
  length_ = 0;
  width_ = 0;
  height_ = 0;
  delete head_;
}

/**
 * Makes the current object into a copy of the parameter:
 * All member variables should have the same value as
 * those of other, but the memory should be completely
 * independent. This function is used in both the copy
 * constructor and the assignment operator for Chains.
 */
void Chain::copy(Chain const& other) {
  Node * curr = other.head()->next;
  while (size() != other.size()) {
    insertBack(curr->data);
    curr = curr->next;
  }
}

/**
* Helper function
* Returns head of a given chain
*/
typename Chain::Node * Chain::head() const{
  return head_;
}

Chain::Node * Chain::getNext(){

  if (length_ != 0) {
    Node * r = head_->next;
    Node * r2 = r->next;

    head_->next = r2;
    r2->prev = head_;
    length_--;
    return r;

  } else {return NULL;
}

}
