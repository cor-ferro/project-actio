#ifndef SRC_MEMORY_SINGLYLINKEDLIST_H_
#define SRC_MEMORY_SINGLYLINKEDLIST_H_


namespace memory {
	template <class T>
	class SinglyLinkedList {
	public:
		struct Node {
			T data;
			Node * next;
		};

		Node * head;

	public:
		SinglyLinkedList();

		void insert(Node * previousNode, Node * newNode);
		void remove(Node * previousNode, Node * deleteNode);
	};
}

#include "singlylinkedlistimpl.h"


#endif /* SRC_MEMORY_SINGLYLINKEDLIST_H_ */
