class Node(object):
    def __init__(self, value):
        self.value = value
        self.left = None
        self.right = None
        self.parent = None
        self.color = "red"

class RedBlackTree(object):
    def __init__(self):
        self.root = None
        self.size = 0

    def __len__(self):
        return self.size

    def l_rotate(self, node):
        t_parent = node.parent
        t_right = node.right
        node.right = t_right.left
        if node.right is not None:
            node.right.parent = node
        t_right.left = node
        node.parent = t_right
        t_right.parent = t_parent
        if t_parent is None:
            self.root = t_right
        else:
            if t_parent.left is node:
                t_parent.left = t_right
            else:
                t_parent.right = t_right

    def r_rotate(self, node):
        t_parent = node.parent
        t_left = node.left
        node.left = t_left.right
        if node.left is not None:
            node.left.parent = node
        t_left.right = node
        node.parent = t_left
        t_left.parent = t_parent
        if t_parent is None:
            self.root = t_left
        else:
            if t_parent.left is node:
                t_parent.left = t_left
            else:
                t_parent.right = t_left

    def insert(self, node):
        t_root = self.root
        t_node = None

        while t_root is not None:
            t_node = t_root
            if node.value == t_node.value:
                return
            elif node.value < t_node.value:
                t_root = t_root.left
            else:
                t_root = t_root.right

        # print("temp node:", type(t_node))
        # print("inserting node: ", node.value)
        if t_node is None:
            self.root = node
            node.color = "black"
        elif node.value < t_node.value:
            t_node.left = node
            node.parent = t_node
        else:
            t_node.right = node
            node.parent = t_node

        self.insert_balance(node)
        self.size += 1
    
    def insert_balance(self, node):
        if node.value == self.root.value:
            return

        while node.parent is not None and node.parent.color == 'red':
            if node.parent is node.parent.parent.left:
                t_uncle = node.parent.parent.right
                if t_uncle is not None and t_uncle.color == 'red':
                    node.parent.color = 'black'
                    t_uncle.color = 'black'
                    node.parent.parent.color = 'red'
                    node = node.parent.parent
                else:
                    if node is node.parent.right:
                        node = node.parent
                        self.l_rotate(node)
                    node.parent.color = 'black'
                    node.parent.parent.color = 'red'
                    self.r_rotate(node.parent.parent)
            else:
                t_uncle = node.parent.parent.left
                if t_uncle is not None and t_uncle.color == 'red':
                    node.parent.color = 'black'
                    t_uncle.color = 'black'
                    node.parent.parent.color = 'red'
                    node = node.parent.parent
                else:
                    if node is node.parent.left:
                        node = node.parent
                        self.r_rotate(node)
                    node.parent.color = 'black'
                    node.parent.parent.color = 'red'
                    self.l_rotate(node.parent.parent)

        self.root.color = 'black'

