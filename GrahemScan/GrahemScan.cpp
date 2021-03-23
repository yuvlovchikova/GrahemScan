#include <iostream>
#include <fstream>
#include <vector>
#include<algorithm>

template <typename T>
class Stack {
public:
	Stack(int capacity) {
		if (capacity > max_size())
			throw std::exception("Вместимость стека не может быть больше max_size");
		this->capacity_ = capacity;
		this->size_ = 0;
		this->arr_ = new T * [capacity];
	}

	static constexpr int max_size() { return 1000; }

	int size() {
		return this->size_;
	}

	bool isEmpty() {
		return this->size() == 0;
	}

	void push(T* item) {
		if (this->size_ == this->capacity_) throw std::exception("Нельзя добавить ещё один элемент, так как достигнут максимальный размер стека");

		this->arr_[this->size_] = item;
		this->size_++;
	}

	T* pop() {
		if (this->size_ < 1) throw std::exception("Нельзя удалить элемент из пустого стека");
		--this->size_;;
		return this->arr_[this->size_];
	}

	T* top() {
		if (this->size_ < 1) throw std::exception("Нельзя вернуть элемент из пустого стека");

		return this->arr_[this->size_ - 1];
	}

	T* nextToTop() {
		if (this->size() < 2) throw std::exception("Нельзя вернуть предпоследний элемент, так как размер стека меньше двух");

		return this->arr_[this->size_ - 2];
	}

private:
	T** arr_;
	int capacity_;
	int size_;
};


class Point
{
public:

	Point(int x, int y)
	{
		this->x_ = x;
		this->y_ = y;
	}

	int getX() {
		return this->x_;
	}

	int getY() {
		return this->y_;
	}

private:
	int x_;
	int y_;
};

bool IsLeftRotation(Point* p1, Point* p2, Point* p3)
{
	try
	{
		return ((p2->getX() - p1->getX()) * (p3->getY() - p2->getY()) - (p3->getX() - p2->getX()) * (p2->getY() - p1->getY()) > 0);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
		return true;
	}
}


Stack<Point> GetConvexHull(std::vector<Point*> dots)
{
	int length = dots.size();
	Point* startPoint = dots[0];
	for (int i = 1; i < length; i++)
	{
		if (((dots[i]->getY() == startPoint->getY()) && (dots[i]->getX() < startPoint->getX())) || (dots[i]->getY() < startPoint->getY())) {
			startPoint = dots[i];
		}
	}
	std::sort(dots.begin(), dots.end(), [startPoint](Point* point_1, Point* point_2) {
		return (atan2(startPoint->getY() - point_1->getY(), startPoint->getX() - point_1->getX()) <
			atan2(startPoint->getY() - point_2->getY(), startPoint->getX() - point_2->getX())); });

	Stack<Point> stack = Stack<Point>(length);
	stack.push(dots[0]);
	stack.push(dots[1]);

	for (int i = 2; i < length; i++)
	{
		while ((stack.size() > 2) && (!IsLeftRotation(stack.nextToTop(), stack.top(), dots[i])))
		{
			stack.pop();
		}
		stack.push(dots[i]);
	}
	return stack;
}


int main(int argc, char* argv[])
{
	try
	{
		std::string bypassOrder = std::string(argv[1]);
		std::string format = std::string(argv[2]);
		std::string pathInput = std::string(argv[3]);
		std::string pathOutput = std::string(argv[4]);
		//чтение из файла
		std::ifstream input_text{ pathInput };
		std::ofstream output_text{ pathOutput };

		int N;
		input_text >> N;
		if (input_text.fail())
			throw std::exception("Плохой формат"); //не могла придумать сообщение лучше

		std::vector<Point*> dots = std::vector<Point*>();
		for (int i = 0; i < N; i++)
		{
			int x, y;
			input_text >> x;
			input_text >> y;
			if (input_text.fail())
				throw std::exception("Плохой формат");
			Point* point = new Point(x, y);
			dots.push_back(point);
		}

		Stack<Point> stack = GetConvexHull(dots);

		std::vector<Point> stackOrdered = std::vector<Point>();
		int stackSize = stack.size();
		for (int i = 0; i < stackSize; i++)
		{
			Point p = *stack.pop();
			stackOrdered.push_back(p);
		}

		if (bypassOrder == "cw")
			std::reverse(stackOrdered.begin() + 1, stackOrdered.end());
		//stackOrdered.Skip(1).Reverse().Prepend(stackOrdered.First()).ToArray();
		if (!(bypassOrder == "cw" || bypassOrder == "cc"))
			throw std::exception("Неверный первый аргумент");

		if (!(format == "plain" || format == "wkt"))
			throw std::exception("Неверный второй аргумент");

		if (format == "plain")
		{
			output_text << stackOrdered.size() << "\n";
			for (int i = 0; i < stackOrdered.size(); i++)
			{
				output_text << stackOrdered[i].getX() << " " << stackOrdered[i].getY() << "\n";
			}
		}

		if (format == "wkt")
		{
			output_text << "MULTIPOINT (";
			for (int i = 0; i < dots.size() - 1; i++)
			{
				output_text << "(" + dots[i]->getX() << " " << dots[i]->getY() << "), ";
			}
			output_text << "(" << dots[dots.size() - 1]->getX() << " " << dots[dots.size() - 1]->getY() << ")) \nPOLYGON ((";
			for (int i = 0; i < stackOrdered.size(); i++)
			{
				output_text << stackOrdered[i].getX() << " " << stackOrdered[i].getY() << ", ";
			}
			output_text << stackOrdered[0].getX() << " " << stackOrdered[0].getY() << "))";
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}
