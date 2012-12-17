#include <iostream>
#include <vector>
using namespace std;

void func( int array[], size_t size )
{
	//der richtige Typ von i ist size_t
	for( size_t i = 0; i < size; ++i )
		array[ i ] = i;
}

int main()
{
	double a;
	std::vector<int> array(100);
	func( array.data(), array.size() );

	a=array[20]/40.; //a=0.5  
	cout<<a<<endl;
}
