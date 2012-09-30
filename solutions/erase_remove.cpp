#include <vector>
#include <algorithm>
#include <iostream>

int main()
{
	std::vector<int> elements = {1, 2, 3, 4, 5};

	//Iteriert über den vector und entfernt effizient die gewünschten Elemente.
	//Effizient heißt hier, dass der Aufwand für das Entfernen pro Element konstant ist. Der naive Ansatz ist üblicherweise ein erase(it) pro Element, was lineare Laufzeit hat.
	elements.erase(
		std::partition(
			elements.begin(),
			elements.end(),
			[](int &element) -> bool
		{
			//Einschränkungen: Der vector darf hier nicht in seiner Größe verändert
			//werden, weil sonst die Iteratoren ungültig würden.
			//Während des Iterierens kann sich die Reihenfolge der Elemente ändern.
		   
			//..
		   
			if (element % 2)
			{
				//false für Entfernen
				return false;
			}
		   
			//..
			return true;
		}),
		elements.end());
	
	std::sort(elements.begin(), elements.end());
	for (auto i = elements.begin(); i != elements.end(); ++i)
	{
		std::cout << *i << "\n";
	}
}
