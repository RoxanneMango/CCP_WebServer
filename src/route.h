#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <string>

typedef struct Route
{
	std::string url;
	std::string PATH;
	bool isPublic;
} Route;

class RouteFactory
{
public:
	RouteFactory()
	{}
	~RouteFactory()
	{}
	static Route * create(const char * url, const char * PATH, bool isPublic = 0)
	{
		return new Route {.url = std::string(url), .PATH = std::string(PATH), .isPublic = isPublic};
	}
	static Route * create(std::string url, std::string PATH, bool isPublic = 0)
	{
		return new Route {.url = url, .PATH = PATH, .isPublic = isPublic};
	}
	static void load(std::vector<Route *> & routes, const char * PATH)
	{
		routes.erase(routes.begin(), routes.end());
		FILE * FP = NULL;
		try
		{
			if((FP = fopen(PATH, "r")) > 0)
			{
				// parser code here
				bool isKey = true;
				char ch = 0;
				
				std::string key = "";
				std::string value = "";
				bool permissionFlag = false;
				
				bool isPermission = false;
				while((ch = fgetc(FP)) != EOF)
				{
					if(ch == ':')
					{
						isKey = false;
					}
					else if(ch == ';')
					{
						isKey = true;
						isPermission = false;
						if(key.size() && value.size())
						{
							routes.push_back(RouteFactory::create(key, value, permissionFlag));
						}
						key.clear();
						value.clear();
					}
					else if(ch == ',')
					{
						isPermission = true;
					}
					else if((ch>='a' && ch<='z')||(ch>='A' && ch<='Z')||(ch>='0' && ch<='9')||(ch=='-')||(ch=='_')||(ch=='.')||(ch=='/'))
					{
						if(isKey)
						{
							key += ch;
						}
						else
						{
							if(isPermission)
							{
								permissionFlag = ch - '0';
							}
							else
							{
								value += ch;
							}
						}
					}
				}
				fclose(FP);
			}
			else
			{
				throw "Could not open file";
			}
		}
		catch(const char * exception)
		{
			printf("Exception : %s\n", exception);
		}
	}
};

#endif // ROUTE_H