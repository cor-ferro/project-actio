#ifndef SRC_MEMORY_STACKLINKEDLIST_H_
#define SRC_MEMORY_STACKLINKEDLIST_H_


namespace memory {
	template <class T>
	class StackLinkedList {
	public:
		struct Node {
			T data;
			Node* next;
		};

		Node* head;
	public:
		StackLinkedList();

		void push(Node * newNode);
		Node* pop();
	private:
		StackLinkedList(StackLinkedList &stackLinkedList);
	};
}

#include "stacklinkedlistimpl.h"

#endif /* SRC_MEMORY_STACKLINKEDLIST_H_ */
