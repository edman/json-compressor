/* generate_random.cpp

*/

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>

std::string addelement(char type)
{
	double ranval;
	int ch, len;
	unsigned long long seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine eng(seed);
	std::stringstream str;
	std::uniform_int_distribution<int> chardist(1, 62), intdist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()), lendist(1, 8);
	std::uniform_real_distribution<double> realdist(0.0, 1.0);

	/* Value part. */
	switch(type)
	{
		/* Boolean case. Use another random distribution. */
		case 'b':
		case 'B':
			ranval = realdist(eng);
			if(ranval >= 0.5)
			{
				str << "true";
			}
			else
			{
				str << "false";
			}
			break;
		/* Integer case. Generate a new integer. */
		case 'i':
		case 'I':
			str << std::to_string(intdist(eng));
			break;
		/* String case. Same mechanism as key. */
		case 's':
		case 'S':
			str << "\"";
			len = lendist(eng);
			for(int i = 0; i < len; i++)
			{
				ch = chardist(eng);
				/* Number. */
				if(ch < 11)
				{
					str << (char)(47 + ch);
				}
				/* Upper case. */
				else if(ch < 37)
				{
					str << (char)(54 + ch);
				}
				/* Lower case. */
				else
				{
					str << (char)(60 + ch);
				}
			}
			str << "\"";
			break;
		/* Null case. */
		default:
			str << "null";
			break;
	}

	return str.str();
}

void usage(char *argv0)
{
	std::cerr << "Usage: " << argv0 << " <# of elements> <type> <output file>" << std::endl;
	std::cerr << "<type> " << std::endl << "b: bool / i: int / n: null / o: empty / s: string" << std::endl;
}

int main(int argc, char **argv)
{
	/* Parse arguments. */
	if(argc != 4)
	{
		usage(argv[0]);
		return 1;
	}
	long long num;
	num = std::atoll(argv[1]);
	if(num < 1)
	{
		std::cerr << "Invalid number of elements." << std::endl;
		usage(argv[0]);
		return 1;
	}
	char types[11] = "bBiInNsS";
	for(int i = 0; i < 10; i++)
	{
		if(i == 9)
		{
			std::cerr << "Invalid type." << std::endl;
			usage(argv[0]);
			return 1;
		}
		if(argv[2][0] == types[i]) break;
	}

	/* Open file. */
	std::ofstream out;
	out.open(argv[3]);
	if(!out.is_open())
	{
		std::cerr << "Output file cannot be opened." << std::endl;
		return 1;
	}

	/* Generate random balanced parantheses.*/
	bool first = true, deep = false;
	double ranval;
	long long excess = 1;
	unsigned long long seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine eng(seed);
	std::uniform_real_distribution<double> dist(0.0, 1.0);

	/* First case. */
	out << "{\"0\": " << addelement(argv[2][0]) << std::endl;

	for(long long i = 1; i < num; i++)
	{
		/* If "excess" is not less than "num" - "i", then the next one should be a closed paranthesis. */
		if(excess >= num - i)
		{
			excess--;
			out << "}" << std::endl;
			continue;
		}

		ranval = dist(eng);
		/* Add depth. */
		if(ranval > 5.0 / 6.0)
		{
			excess++;
			if(first || deep)
			{
				out << ",";
			}
			out << "\"" << i << "\": {";
			deep = true;
		}
		/* Add element. */
		else if(ranval > 1.0 / 6.0)
		{
			if(!deep)
			{
				out << ",";
			}
			deep = false;
			out << "\"" << i << "\": " << addelement(argv[2][0]);
		}
		else
		{
			if(excess > 1)
			{
				excess--;
				out << "}";
				deep = false;
			}
			/* Should not be end of the document. */
			else
			{
				i--;
				continue;
			}
		}
		out << std::endl;
		first = true;
	}

	/* Close file. */
	out.close();

	return 0;
}
