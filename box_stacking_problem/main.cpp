#include <iostream>
#include <fstream>

using namespace std;

template<typename Element>
class vector {
public:
	vector() : array_(NULL), size_(0) {}
	vector(const vector& obj) : array_(NULL), size_(0) {
		size_ = obj.size_;
		if (size_ <= 0) {
			return;
		}
		array_ = new Element[size_];
		for (int i = 0; i < size_; ++i) {
			array_[i] = obj.array_[i];
		}
	}
	~vector() {
		clear();
	}
	vector& operator=(const vector& obj) {
		clear();
		size_ = obj.size_;
		if (size_ <= 0) {
			return *this;
		}
		array_ = new Element[size_];
		for (int i = 0; i < size_; ++i) {
			array_[i] = obj.array_[i];
		}
		return *this;
	}
	void push_back(const Element& ele) {
		Element* new_array = new Element[size_ + 1];
		for (int i = 0; i < size_; ++i) {
			new_array[i] = array_[i];
		}
		new_array[size_] = ele;
		if (array_) {
			delete[] array_;
		}
		array_ = new_array;
		++size_;
	}
	Element& operator[](const int& at) {
		return array_[at];
	}
	const Element& operator[](const int& at) const {
		return array_[at];
	}
	int size() const {
		return size_;
	}
	bool empty() const {
		return size_ <= 0;
	}
	void clear() {
		size_ = 0;
		if (array_) {
			delete[] array_;
			array_ = NULL;
		}
	}
private:
	Element* array_;
	int size_;
};

//////////////////////////////////////////

struct Box {
	Box(const int& x, const int& y, const int& z, const int& i) : id(i) {
		sides.push_back(x);
		sides.push_back(y);
		sides.push_back(z);
	}
	void ChooseL() {
		if (sides[0] < sides[1]) {
			swap(sides[0], sides[1]);
		}
		if (sides[0] < sides[2]) {
			swap(sides[0], sides[2]);
		}
		if (sides[1] < sides[2]) {
			swap(sides[1], sides[2]);
		}
	}
	void ChooseM() {
		ChooseL();
		swap(sides[0], sides[1]);
	}
	void ChooseS() {
		ChooseL();
		swap(sides[0], sides[2]);
	}
	bool CanBeAbove(const Box& box) {
		if ((sides[1] >= box.sides[1]) && (sides[2] >= box.sides[2])) {
			return true;
		}
		if ((sides[1] >= box.sides[2]) && (sides[2] >= box.sides[1])) {
			return true;
		}
		return false;
	}
	vector<int> sides;
	int id;
};

/////////////////////////////////////////////////////////////////////////////////

struct Node {
	Node() : ref_height(0), visited(false) {}
	int ref_height;
	bool visited;
	vector<Node*> aboves;
	Box* box;
};

void FindAboves(vector<Node>& nodes) {
	for (int i = 0; i < nodes.size(); ++i) {
		for (int j = 0; j < nodes.size(); ++j) {
			if (nodes[i].box->id == nodes[j].box->id) {
				continue;
			}
			if (nodes[i].box->CanBeAbove(*nodes[j].box)) {
				nodes[i].aboves.push_back(&nodes[j]);
			}
		}
	}
}

void CalcRefHeight(Node& node) {
	if (node.visited) {
		return;
	}
	node.visited = true;
	if (node.aboves.empty()) {
		node.ref_height = node.box->sides[0];
		return;
	}
	for (int i = 0; i < node.aboves.size(); ++i) {
		CalcRefHeight(*node.aboves[i]);
	}
	int max_ref_height = 0;
	for (int i = 0; i < node.aboves.size(); ++i) {
		if (node.aboves[i]->ref_height > max_ref_height) {
			max_ref_height = node.aboves[i]->ref_height;
		}
	}
	node.ref_height = node.box->sides[0] + max_ref_height;
}

//////////////////////////////////////////////////////////////////

struct Path {
	Path() : height(0) {}
	vector<Node*> nodes;
	int height;
};

void UpdateMaxHeight(Node& node, const Path& path, int& max_height) {
	if ((node.ref_height + path.height) < max_height) {
		return;
	}
	for (int i = 0; i < path.nodes.size(); ++i) {
		if (path.nodes[i]->box->id == node.box->id) {
			if (path.height > max_height) {
				max_height = path.height;
			}
			return;
		}
	}
	if (node.aboves.empty()) {
		int height = path.height + node.box->sides[0];
		if (height > max_height) {
			max_height = height;
		}
		return;
	}
	Path new_path = path;
	new_path.height += node.box->sides[0];
	new_path.nodes.push_back(&node);
	for (int i = 0; i < node.aboves.size(); ++i) {
		UpdateMaxHeight(*node.aboves[i], new_path, max_height);
	}
}

//////////////////////////////////////////////////////////////////////////////

int main() {
	ifstream is("C:/Users/codincodee/Desktop/data.txt");
	int total_case;
	is >> total_case;
	for (int case_id = 1; case_id <= total_case; ++case_id) {
		int total_box;
		is >> total_box;
		vector<Node> nodes;
		for (int box_id = 0; box_id < total_box; ++box_id) {
			int x, y, z;
			is >> x;
			is >> y;
			is >> z;
			Node node;
			node.box = new Box(x, y, z, box_id);
			node.box->ChooseL();
			nodes.push_back(node);
			node.box = new Box(x, y, z, box_id);
			node.box->ChooseM();
			nodes.push_back(node);
			node.box = new Box(x, y, z, box_id);
			node.box->ChooseS();
			nodes.push_back(node);
		}
		FindAboves(nodes);
		for (int i = 0; i < nodes.size(); ++i) {
			CalcRefHeight(nodes[i]);
		}
		Path path;
		int max_height = 0;
		for (int i = 0; i < nodes.size(); ++i) {
			UpdateMaxHeight(nodes[i], path, max_height);
		}
		for (int i = 0; i < nodes.size(); ++i) {
			delete nodes[i].box;
		}
		cout << "#" << case_id << " " << max_height << endl;
	}
	is.close();
}
