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
	std::uniform_int_distribution<int> chardist(1, 62), intdist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()), lendist(10, 20);
	std::uniform_real_distribution<double> realdist(0.0, 1.0);

	/* Value part. */
	switch(type)
	{
		/* Array case. */
		case 'a':
		case 'A':
			str << "[";
			break;
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
		/* Double case. Generate a new double. */
		case 'd':
		case 'D':
			str << std::to_string(realdist(eng));
			break;
		/* Integer case. Generate a new integer. */
		case 'i':
		case 'I':
			str << std::to_string(intdist(eng));
			break;
		/* Object case. */
		case 'o':
		case 'O':
			str << "{";
			break;
		/* String case. Same mechanism as key. */
		case 's':
		case 'S':
			str << "\"";
			len = lendist(eng); //Change this to 10 ~ 20.
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
	std::cerr << "<type> " << std::endl << "a: array / b: bool / d: double / i: int / n: null / o: object / r: random / s: string" << std::endl;
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
	char types[17] = "aAbBdDiInNoOrRsS";
	for(int i = 0; i < 18; i++)
	{
		if(i == 17)
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
	bool deep = true, first = true;
	char nexttype;
	double ranval;
	long long curr = 1, excess = 1, keyval[17];
	unsigned long long seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine eng(seed);
	std::uniform_int_distribution<int> chardist(97, 122);
	std::uniform_real_distribution<double> dist(0.0, 1.0);

	/* First case. */
	if(argv[2][0] != 'a' && argv[2][0] != 'A' && argv[2][0] != 'o' && argv[2][0] != 'O')
	{
		excess++;
		out << "{\"1\": {" << std::endl;
	}
	else
	{
		curr--;
		out << "{" << std::endl;
	}
	keyval[1] = 2;
	for(int i = 2; i < 17; i++)
	{
		keyval[i] = 1;
	}

	while(curr < num)
	{
		if(excess > 15)
		{
			deep = false;
			keyval[excess] = 1;
			excess--;
			if(argv[2][0] == 'a' || argv[2][0] == 'A')
			{
				out << "]";
			}
			else
			{
				out << "}";
			}
			continue;
		}

		ranval = dist(eng);
		/* Add depth. */
		if(ranval > 3.0 / 5.0)
		{
			curr++;
			if(!deep)
			{
				out << ",";
			}
			if(argv[2][0] == 'a' || argv[2][0] == 'A')
			{
				if(excess != 1)
				{
					out << "[" << std::endl;
				}
				else
				{
					out << "\"" << keyval[excess] << "\": [" << std::endl;
				}
			}
			else
			{
				out << "\"" << keyval[excess] << "\": {" << std::endl;
			}
			deep = true;
			first = false;
			keyval[excess]++;
			excess++;
		}
		/* Add element. */
		else if(ranval > 1.0 / 2.0)
		{
			curr++;
			if(!deep)
			{
				out << ",";
			}
			deep = false;
			if(argv[2][0] == 'a' || argv[2][0] == 'A')
			{
				excess++;
				if(excess > 2)
				{
					out << addelement(argv[2][0]) << std::endl;
				}
				else
				{
					out << "\"" << keyval[excess] << "\": " << addelement(argv[2][0]) << std::endl;
				}
				deep = true;
			}
			else
			{
				if(argv[2][0] == 'o' || argv[2][0] == 'O')
				{
					excess++;
					deep = true;
				}
				if(argv[2][0] == 'r' || argv[2][0] == 'R')
				{
					out << "\"" << keyval[excess] << "\": ";
					ranval = dist(eng);
					if(ranval > 0.8)
					{
						out << addelement('b') << std::endl;
					}
					else if(ranval > 0.6)
					{
						out << addelement('d') << std::endl;
					}
					else if(ranval > 0.4)
					{
						out << addelement('i') << std::endl;
					}
					else if(ranval > 0.2)
					{
						out << addelement('n') << std::endl;
					}
					else
					{
						out << addelement('s') << std::endl;
					}
				}
				else
				{
					out << "\"" << keyval[excess] << "\": " << addelement(argv[2][0]) << std::endl;
				}
			}
			first = false;
			keyval[excess]++;
		}
		else
		{
			if(excess == 1 || first)
			{
				continue;
			}
			if(argv[2][0] == 'a' || argv[2][0] == 'A')
			{
				out << "]" << std::endl;
			}
			else
			{
				out << "}" << std::endl;
			}
			deep = false;
			keyval[excess] = 1;
			excess--;
		}
	}

	while(excess > 1)
	{
		if(argv[2][0] == 'a' || argv[2][0] == 'A')
		{
			out << "]" << std::endl;
		}
		else
		{
			out << "}" << std::endl;
		}
		excess--;
	}
	out << "}" << std::endl;

	std::cerr << keyval[1] << std::endl;

	/* Close file. */
	out.close();

	return 0;
}
