#include "database_server.h"

DatabaseServer::DatabaseServer(unsigned int id, Socket & listener, Socket & client, unsigned int bufferSize) : 
	Server(listener, client)
{
	this->id = id;
	setBufferSize(bufferSize);
}
DatabaseServer::~DatabaseServer()
{}

void
DatabaseServer::processQuery()
{
	// sanitize input from bufferPtr and do stuff with it . . .	
	printf("Incoming query ->\n\t%s\n", receiveBuffer); fflush(stdout);
	
	bufferPtr = receiveBuffer;

	try
	{
		int size = 0;
		while(*bufferPtr != ' ') 
		{ 
			*bufferPtr = tolower(*bufferPtr);
			bufferPtr++;
			size++; 
		}
		if(!strncmp(bufferPtr-size, "select", size))
		{
			//printf("select: %s\n", &select()[0]);
			sprintf(buffer, "%s", &select()[0]);
		}
		else if(!strncmp(bufferPtr-size, "insert", size))
		{
			int size = 0; bufferPtr++;
			while(*bufferPtr != ' ') 
			{ 
				*bufferPtr = tolower(*bufferPtr);
				bufferPtr++;
				size++; 
			}
			if(!strncmp(bufferPtr-size, "into", size))
			{
				sprintf(buffer, "%d", insert());
			}
		}
		else if(!strncmp(bufferPtr-size, "update", size))
		{
			sprintf(buffer, "%d", update());
		}
	}

	catch(const char * exception)
	{
		printf("Error: %s\n", exception);
	}

}

int
DatabaseServer::load(const char * path)
{
	try
	{
		FILE * FP = NULL;
		if((FP = fopen(path, "r")) == NULL)
		{
			throw "Could not open file.";
		}
		
		const unsigned int commandSize = 32;
		char command[commandSize];
		memset(command, 0, commandSize);

		// skip white lines
		char c = tolower(fgetc(FP));
		while( (c == ' ') || (c == '\n') || (c == '\t') )
		{
			c = fgetc(FP); 
		}
		while(c != EOF)
		{
			memset(command, 0, commandSize);
			if(!isLetter(c) || (c == ';'))
			{
				c = fgetc(FP); 
				continue;
			}
			for(unsigned int i = 0; isLetter(c) && (c != ';') && (i < commandSize); ++i )
			{
				command[i] = tolower(c);
				c = fgetc(FP); 
				
			}
			if(!(strcmp(command, "use")))
			{
				memset(command, 0, commandSize);
				if(!databases.size())
					throw "Cannot USE non-existent database.";
				
				c = fgetc(FP); 
				bool isFound = false;
				for(unsigned int i = 0; i < databases.size(); ++i)
				{
					for( unsigned int i = 0; isLetter(c) && (c != ';') && (i < commandSize); ++i )
					{
						command[i] = c;
						c = fgetc(FP); 
					}
					if(!strcmp(&databases[i].name[0], command))
					{
						index = i;
						isFound = true;
						//printf("USE %s\n", command); fflush(stdout);
						break;
					}
				}
				if(!isFound)
				{
					index = -1;
					throw "Could not find database to USE.";
				}
			}
			
			else if(!(strcmp(command, "create")))
			{
				memset(command, 0, commandSize);
				c = fgetc(FP); 
				for( unsigned int i = 0; isLetter(c) && (i < commandSize); ++i )
				{
					//printf("%c", c); fflush(stdout);
					command[i] = tolower(c);
					c = fgetc(FP); 
				}
				if(!(strcmp(command, "database")))
				{
					memset(command, 0, commandSize);
					c = fgetc(FP); 
					for( unsigned int i = 0; isLetter(c) && (c != ';') && (i < commandSize); ++i )
					{
						command[i] = c;
						c = fgetc(FP); 
					}
					//printf("CREATE DATABASE %s\n", command); fflush(stdout);
					databases.push_back(SQL_Database(std::string(command)));
				}
				else if(!(strcmp(command, "table")))
				{
					memset(command, 0, commandSize);
					c = fgetc(FP); 
					for( unsigned int i = 0; isLetter(c) && (c != ';') && (i < commandSize); ++i )
					{
						//printf("%c", c); fflush(stdout);
						command[i] = c;
						c = fgetc(FP); 
					}
					if(index < 0)
					{
						throw "Cannot create table for non-existent database.";
					}
					//printf("CREATE TABLE %s\n", command); fflush(stdout);
					databases[index].tables.push_back(Table(std::string(command)));
					
					// now add columns
					int even = 2;
					while(c != ';')
					{
						memset(command, 0, commandSize);
						
						if(!isLetter(c) && !isNumber(c))
						{
							c = fgetc(FP); 
							continue;
						}
						
						for( unsigned int i = 0; (isLetter(c) || isNumber(c) || (((c == '(') || (c == ')')))) && (i < commandSize); ++i )
						{
							command[i] = c;
							c = fgetc(FP); 
						}
						if(!(even%2))
						{
							databases[index].tables[databases[index].tables.size()-1].columns.push_back(Column(std::string(command)));
						}
						c = fgetc(FP); 
						even += 1;
					}
				}
			}

			else if(!(strcmp(command, "insert")))
			{
				memset(command, 0, commandSize);
				c = fgetc(FP); 
				for( unsigned int i = 0; isLetter(c) && (i < commandSize); ++i, c = fgetc(FP) )
				{
					command[i] = tolower(c);
					//c = fgetc(FP);
				}
				if(!(strcmp(command, "into")))
				{
					memset(command, 0, commandSize);
					c = fgetc(FP); 
					for( unsigned int i = 0; isLetter(c) && (c != ';') && (i < commandSize); ++i )
					{
						command[i] = c;
						c = fgetc(FP); 
					}

					if(!databases[index].tables.size())
						throw "Could not INSERT INTO non-existent table";
					
					for(unsigned int table_i = 0; table_i < databases[index].tables.size(); ++table_i)
					{
						if(!strcmp(&databases[index].tables[table_i].name[0], command))
						{
							//printf("INSERT INTO TABLE %s\n", command); fflush(stdout);
							
							std::vector<int> argIndexes;
							// keep iterating until end of statement
							while(c != ')')
							{
								memset(command, 0, commandSize);
								
								if(!isLetter(c) && !isNumber(c))
								{
									c = fgetc(FP); 
									continue;
								}
								
								for( unsigned int i = 0; (isLetter(c) || isNumber(c)) && (i < commandSize); ++i )
								{
									command[i] = c;
									c = fgetc(FP); 
								}
								for(unsigned int i = 0; i < databases[index].tables[table_i].columns.size(); ++i)
								{
									//printf("name: %s ; command: %s\n", &databases[index].tables[table_i].columns[i].name[0], command);
									if(!strcmp(&databases[index].tables[table_i].columns[i].name[0], command))
									{	
										//printf("index = %d\n", i); fflush(stdout);
										argIndexes.push_back(i);
										break;
									}
								}
								c = fgetc(FP); 
							}
							while(!isLetter(c)) { c = fgetc(FP);  }
							memset(command, 0, commandSize);
							for( unsigned int i = 0; isLetter(c) && (i < commandSize); ++i )
							{
								command[i] = tolower(c);
								c = fgetc(FP); 
							}
							//printf("command: %s\n", command); fflush(stdout);
							if(!strcmp(command, "values"))
							{
								c = fgetc(FP);
								//printf("%c", c); fflush(stdout);
								while(c != ';')
								{
									int arg_i = 0;
									bool isString = false;
									
									while((c == '\'') || !isNumber(c)) { c = fgetc(FP); }
									
									while(c != ')')
									{
										memset(command, 0, commandSize);

										if(c == '\'' && !isString)
										{
											c = fgetc(FP); 
											for(unsigned int i = 0; (c != '\'') && (i < commandSize); ++i)
											{
												command[i] = c;
												c = fgetc(FP); 
											}
											databases[index].tables[table_i].columns[argIndexes[arg_i]].values.push_back(std::string(command));
											arg_i += 1;
											isString = true;
										}
										else
										{
											isString = false;
											if((c == ' ') || (c == '\n') || (c == '\t') || (c == ',') || (c == ')'))
											{
												if(c == ')')
												{
													break;
												}
												else
												{
													c = fgetc(FP); 
													continue;
												}
											}
											for( unsigned int i = 0; ((c == '.') || isNumber(c)) && (i < commandSize); ++i, c = fgetc(FP) )
											{
												command[i] = c;
											}
											//printf("command : %s\n", command);
											//printf("arg_i : %d\n", arg_i); fflush(stdout);
											//printf("size : %d\n", argIndexes.size()); fflush(stdout);
											databases[index].tables[table_i].columns[argIndexes[arg_i]].values.push_back(std::string(command));
											arg_i += 1;
										}
										c = fgetc(FP); 
									}
									c = fgetc(FP); 
								} //printf("end of value;\n");
								//c = fgetc(FP);
							} //printf("end of values;\n");
						} //printf("end of command;\n");
					} //printf("end of tables;\n");
				} //printf("end of INSERT INTO command;\n");
			} //printf("begin of INSERT INTO command;\n");
		} //printf("EOF reached . . .\n");
		fclose(FP);
	}
	catch(const char * exception)
	{
		printf("Error: %s\n", exception);
		return -1;
	}
	// set default database index to 0
	index = 0;
	return 0;
}

int
DatabaseServer::save(const char * path)
{
	FILE * FP = NULL;
	if((FP = fopen(path, "w")) < 0)
	{
		printf("Could not open save file.");
		return -1;
	}

	for(unsigned int i = 0; i < databases.size(); ++i)
	{
		fprintf(FP, "CREATE DATABASE %s\n\n", &databases[i].name[0]);
		fprintf(FP, "USE %s\n\n", &databases[i].name[0]);
		for(unsigned int j = 0; j < databases[i].tables.size(); ++j)
		{
			fprintf(FP, "CREATE TABLE %s\n(\n", &databases[i].tables[j].name[0]);
			for(unsigned int k = 0; k < databases[i].tables[j].columns.size()-1; ++k)
			{
				fprintf(FP, "\t%s STRING,\n", &databases[i].tables[j].columns[k].name[0]);
			}
			fprintf(FP, "\t%s STRING\n);\n", &databases[i].tables[j].columns[databases[i].tables[j].columns.size()-1].name[0]);
			
			fprintf(FP, "INSERT INTO %s\n(\n\t", &databases[i].tables[j].name[0]);
			for(unsigned int k = 0; k < databases[i].tables[j].columns.size()-1; ++k)
			{
				fprintf(FP, "%s, ", &databases[i].tables[j].columns[k].name[0]);
			}
			fprintf(FP, "%s\n)\nVALUES\n", &databases[i].tables[j].columns[databases[i].tables[j].columns.size()-1].name[0]);
			
			/* * * * * * * * * * * * * * * * * * * * * *\
			* 										  	*
			*	for(all columns)					  	*
			*	{									  	*
			*		print value at index x			  	*
			*	}									  	*
			*										  	*
			* [] [] [] [] [] [] [] [] [] [] [] [] [] [] *
			* :: :: :: :: :: :: :: :: :: :: :: :: :: ::	*
			* [] [] [] [] [] [] [] [] [] [] [] [] [] []	*
			* || || || || || || || || || || || || || ||	*
			* [] [] [] [] [] [] [] [] [] [] [] [] [] []	*
			* || || || || || || || || || || || || || ||	*
			* [] [] [] [] [] [] [] [] [] [] [] [] [] []	*
			* || || || || || || || || || || || || || ||	*
			* [] [] [] [] [] [] [] [] [] [] [] [] [] []	*
			* 											*
			\* * * * * * * * * * * * * * * * * * * * * */
			
			unsigned int lastCol = databases[i].tables[j].columns.size()-1;
			unsigned int lastVal = databases[i].tables[j].columns[lastCol].values.size()-1;

			for(unsigned int vi = 0; vi < lastVal; ++vi)
			{
				fprintf(FP, "( ");
				for(unsigned int k = 0; k < lastCol; ++k)
				{
					//printf("???\n"); fflush(stdout);
					//printf("table: %s\n",&databases[i].tables[j].name[0]); fflush(stdout);
					//printf("column: %s\n",&databases[i].tables[j].columns[k].name[0]); fflush(stdout);
					//printf("values size: %u\n",databases[i].tables[j].columns[k].values.size()); fflush(stdout);
					if(isString(&databases[i].tables[j].columns[k].values[vi][0]))
					{
						fprintf(FP, "'%s', ", &databases[i].tables[j].columns[k].values[vi][0]);
					}
					else
					{
						fprintf(FP, "%s, ", &databases[i].tables[j].columns[k].values[vi][0]);
					}
				}
				if(isString(&databases[i].tables[j].columns[lastCol].values[vi][0]))
				{
					fprintf(FP, "'%s' ),\n", &databases[i].tables[j].columns[lastCol].values[vi][0]);
				}
				else
				{
					fprintf(FP, "%s ),\n", &databases[i].tables[j].columns[lastCol].values[vi][0]);
				}
			}
			fprintf(FP, "( ");
			for(unsigned int k = 0; k < lastCol; ++k)
			{
				if(isString(&databases[i].tables[j].columns[k].values[lastVal][0]))
				{
					fprintf(FP, "'%s', ", &databases[i].tables[j].columns[k].values[lastVal][0]);
				}
				else
				{
					fprintf(FP, "%s, ", &databases[i].tables[j].columns[k].values[lastVal][0]);
				}
			}
			if(isString(&databases[i].tables[j].columns[lastCol].values[lastVal][0]))
			{
				fprintf(FP, "'%s' );\n\n", &databases[i].tables[j].columns[lastCol].values[lastVal][0]);
			}
			else
			{
				fprintf(FP, "%s );\n\n", &databases[i].tables[j].columns[lastCol].values[lastVal][0]);
			}
		}
	}
	fclose(FP);
	return 0;
}

std::string
DatabaseServer::select()
{
	// CONVERT CHAR *'s INTO std::string SO THEY CAN BE FREED BEFORE WE ENTER THE CLUSTERFUCK OF LOOPS
	
	std::string selected;	
	try
	{
//		printf("select ");

		int size = 0;

		bufferPtr++;
		while(*bufferPtr != ' ') 
		{ 
			bufferPtr++;
			size++; 
		}
		char * value = (char *) calloc(size+1, sizeof(char));
		strncpy(value, bufferPtr-size, size);
//		std::string value = std::string(value);
		
//		printf("%s ", value);

		bufferPtr++; size = 0;
		while(*bufferPtr != ' ')
		{
			*bufferPtr = tolower(*bufferPtr);
			bufferPtr++;
			size++; 
		}
		if(!strncmp(bufferPtr-size, "from", size))
		{
//			printf("from ");
		
			bufferPtr++; size = 0;
			while(*bufferPtr != ' ') 
			{ 
				bufferPtr++;
				size++; 
			}
			char * table = (char *) calloc(size+1, sizeof(char));
			strncpy(table, bufferPtr-size, size);
//			printf("%s ", table);
			
			bufferPtr++; size = 0;
			while(*bufferPtr != ' ')
			{
				*bufferPtr = tolower(*bufferPtr);
				bufferPtr++;
				size++; 
			}
			if(!strncmp(bufferPtr-size, "where", size))
			{
//				printf("where ");
						
				bufferPtr++; size = 0;
				while(*bufferPtr != ' ')
				{
					bufferPtr++;
					size++; 
				}
				char * value2 = (char *) calloc(size+1, sizeof(char));
				strncpy(value2, bufferPtr-size, size);
//				printf("%s ", value2);
				
				bufferPtr++; size = 0;
				while(*bufferPtr != ' ') 
				{
					bufferPtr++;
					size++; 
				}
				if(!strncmp(bufferPtr-size, "=", size))
				{
//					printf("= ");

					bufferPtr++; size = 0;
					if(*bufferPtr == '\'') bufferPtr++;
					while((*bufferPtr != '\'') && (*bufferPtr != ';'))
					{
						bufferPtr++;
						size++; 
					}
					char * value3 = (char *) calloc(size+1, sizeof(char));
					strncpy(value3, bufferPtr-size, size);
//					printf("%s ", value3); fflush(stdout);
	
					int indexY = -1;
					
					if(!databases.size())
						throw "Cannot SELECT from non-existent database.";
					if(!databases[index].tables.size())
						throw "Cannot SELECT from non-existent database table";
					
					for(unsigned int i = 0; i < databases[index].tables.size(); ++i)
					{
						//printf("name: %s\n", &databases[index].tables[i].name[0]);
						//printf("table: %s\n", table);
						if(!strcmp(&databases[index].tables[i].name[0], table))
						{
//							printf("table found!\n"); fflush(stdout);
							if(!databases[index].tables[i].columns.size())
								throw "Cannot SELECT from non-existent database table column";

							for(unsigned int j = 0; j < databases[index].tables[i].columns.size(); ++j)
							{
								if(!strcmp(&databases[index].tables[i].columns[j].name[0], value2))
								{
//									printf("value2 found!\n"); fflush(stdout);
									for(unsigned int k = 0; k < databases[index].tables[i].columns[j].values.size(); ++k)
									{
										if(!strcmp(&databases[index].tables[i].columns[j].values[k][0], value3))
										{
//											printf("value3 found!\n"); fflush(stdout);
											indexY = k;
											break;
										}
									}
								}
							}
							for(unsigned int j = 0; j < databases[index].tables[i].columns.size(); ++j)
							{
								if(indexY < 0)
								{
									throw "Could not select asked query.";
								}
								if(!strcmp(&databases[index].tables[i].columns[j].name[0], value))
								{
									selected = databases[index].tables[i].columns[j].values[indexY];
									//printf("search result: %s\n", &databases[index].tables[i].columns[j].values[indexY][0]); fflush(stdout);
								}
							}
						}
					}
					free(value3);
				}
				free(value2);
			}
			else
			{
				// without 'where' clause
				// . . .
			}
			free(table);
		}
		free(value);
	}
	
	catch(const char * exception)
	{
		printf("Error: %s\n", exception);
		return std::string("!OK");
	}
	return selected;
}

int
DatabaseServer::insert()
{
//	printf("insert\n");
	
	std::vector<std::string> names;
	std::vector<std::string> values;
	
	unsigned int size = 0;
	unsigned int maxSize = 32;
	char * buff = (char *) calloc(maxSize, sizeof(char));

	// get table name
	size = 0; bufferPtr++;
	while(*bufferPtr != ' ')
	{
		if(size >= maxSize)
		{
			free(buff);
			printf("maxSize for table exceeded\n");
			return -1;
		}
		bufferPtr++;
		size++; 
	}
	memset(buff, 0, maxSize);
	strncpy(buff, bufferPtr-size, size);
	
	std::string table = std::string(buff);

	// get argv
	while(*bufferPtr != ';')
	{
		// step over whitespace
		while(*bufferPtr == ' ') bufferPtr++;

		size = 0;
		while((*bufferPtr != ' ') && (*bufferPtr != '='))
		{
			if(size >= maxSize)
			{
				free(buff);
				printf("maxSize for name exceeded (%d)\n", size);
				return -1;
			}
			bufferPtr++;
			size++; 
		}
		memset(buff, 0, maxSize);
		strncpy(buff, bufferPtr-size, size);
		names.push_back(std::string(buff));
		
		// step over whitespace and '='
		while(*bufferPtr == ' ') bufferPtr++;
		if(*bufferPtr == '=') bufferPtr++;
		while(*bufferPtr == ' ') bufferPtr++;

		size = 0;
		while(*bufferPtr != ',' && *bufferPtr != ';')
		{
			if(size >= maxSize)
			{
				free(buff);
				printf("maxSize for value exceeded (%d)\n", size);
				return -1;
			}
			bufferPtr++;
			size++; 
		}
		memset(buff, 0, maxSize);
		strncpy(buff, bufferPtr-size, size);
		values.push_back(std::string(buff));
		
		// step over whitespace and ','
		while(*bufferPtr == ' ') bufferPtr++;
		if(*bufferPtr == ',') bufferPtr++;

	}
	free(buff);

	if(!databases.size())
	{
		printf("Could not INSERT INTO non-existent database"); fflush(stdout);
		return -1;
	}	
	if(!databases[index].tables.size())
	{
		printf("Could not INSERT INTO non-existent table"); fflush(stdout);
		return -1;
	}
	if(names.size() != values.size())
	{
		printf("Number of names is not equal to number of values."); fflush(stdout);
		return -1;
	}
	
	unsigned int valuesNum;
	if(!databases[index].tables[0].columns.size())
		valuesNum = 0;
	else
		valuesNum = databases[index].tables[0].columns[0].values.size();

	for(unsigned int i = 0; i < databases[index].tables.size(); i++)
	{
		if(!strcmp(&databases[index].tables[i].name[0], &table[0]))
		{
			for(unsigned int j = 0; j < names.size(); ++j)
			{
				for(unsigned int k = 0; k < databases[index].tables[i].columns.size(); ++k)
				{
					if(!strcmp(&databases[index].tables[i].columns[k].name[0], &names[j][0]))
					{
						databases[index].tables[i].columns[k].values.push_back(values[j]);
					}
				}	
			}
		}
	}
	
	// fill in any undefined columns
	for(unsigned int i = 0; i < databases[index].tables.size(); i++)
	{
		for(unsigned int j = 0; j < databases[index].tables[i].columns.size(); ++j)
		{
			if(databases[index].tables[i].columns[j].values.size() == valuesNum)
			{
				if(!strcmp(&databases[index].tables[i].columns[j].name[0], "id"))
				{
					std::string id = std::to_string(databases[index].tables[i].columns[j].values.size());
					databases[index].tables[i].columns[j].values.push_back(id);
				}
				if(!strcmp(&databases[index].tables[i].columns[j].name[0], "registrationDate"))
				{
					// get current time
					const int max = 32;
					time_t currentTime = time(0);
					char registrationTime[max]; 
					strftime(registrationTime, max, "%d:%m:%Y:%X", gmtime(&currentTime));
					
					databases[index].tables[i].columns[j].values.push_back(std::string(registrationTime));
				}
				else
				{
					databases[index].tables[i].columns[j].values.push_back(std::string("null"));
				}
			}
		}
	}	
	return 0;
}

int
DatabaseServer::update()
{
//	printf("update\n"); fflush(stdout);
	
	std::vector<std::string> names;
	std::vector<std::string> values;
	
	unsigned int size = 0;
	unsigned int maxSize = 32;
	char * buff = (char *) calloc(maxSize, sizeof(char));

	// get table name
	size = 0; bufferPtr++;
	while(*bufferPtr != ' ')	// table name
	{
		if(size >= maxSize)
		{
			free(buff);
			printf("maxSize for table exceeded\n");
			return -1;
		}
		bufferPtr++;
		size++; 
	}
	memset(buff, 0, maxSize);
	strncpy(buff, bufferPtr-size, size);
	
	std::string table = std::string(buff);

//	printf("table: %s\n", &table[0]); fflush(stdout);

	size = 0; bufferPtr++;
	while(*bufferPtr != ' ')	// set
	{
		if(size >= maxSize)
		{
			free(buff);
			printf("maxSize for table exceeded\n");
			return -1;
		}
		*bufferPtr = tolower(*bufferPtr);
		bufferPtr++;
		size++; 
	}
	if(strncmp(bufferPtr-size, "set", size))
	{
		free(buff);
		printf("Invalid command.");
		return -1;
	}
	
//	printf("SET\n"); fflush(stdout);
	
	// get argv
	while(*bufferPtr != ';')	// argument list
	{
		// step over whitespace
		while(*bufferPtr == ' ') bufferPtr++;

		size = 0;
		while((*bufferPtr != ' ') && (*bufferPtr != '='))
		{
			if(size >= maxSize)
			{
				free(buff);
				printf("maxSize for name exceeded (%d)\n", size);
				return -1;
			}
			bufferPtr++;
			size++; 
		}
		memset(buff, 0, maxSize);
		strncpy(buff, bufferPtr-size, size);
		names.push_back(std::string(buff));
		
		// step over whitespace and '='
		while(*bufferPtr == ' ') bufferPtr++;
		if(*bufferPtr == '=')
		{
			bufferPtr++;
		}
		else
		{
			free(buff);
			printf("Invalid character.");
			return -1;
		}
		while(*bufferPtr == ' ') bufferPtr++;

		size = 0;
		while(*bufferPtr != ',' && *bufferPtr != ';')
		{
			if(size >= maxSize)
			{
				free(buff);
				printf("maxSize for value exceeded (%d)\n", size);
				return -1;
			}
			bufferPtr++;
			size++; 
		}
		memset(buff, 0, maxSize);
		strncpy(buff, bufferPtr-size, size);
		values.push_back(std::string(buff));
		
		// step over whitespace and ','
		while(*bufferPtr == ' ') bufferPtr++;
		if(*bufferPtr == ',') bufferPtr++;

	}
	// step over ';'
	bufferPtr++;

//	printf("ARG_LIST\n"); fflush(stdout);

	while(*bufferPtr == ' ') bufferPtr++;

	size = 0;
	while(*bufferPtr != ' ')	// where
	{
		if(size >= maxSize)
		{
			free(buff);
			printf("maxSize for where exceeded\n");
			return -1;
		}
		*bufferPtr = tolower(*bufferPtr);
		
		bufferPtr++;
		size++;
	}
	if(strncmp(bufferPtr-size, "where", size))
	{
		free(buff);
		printf("Invalid command.");
		return -1;
	}
	
//	printf("WHERE\n"); fflush(stdout);

	while(*bufferPtr == ' ') bufferPtr++;

	size = 0;
	while(*bufferPtr != ' ')	// column
	{
		if(size >= maxSize)
		{
			free(buff);
			printf("maxSize for column exceeded\n");
			return -1;
		}
		bufferPtr++;
		size++; 
	}
	memset(buff, 0, maxSize);
	strncpy(buff, bufferPtr-size, size);
	std::string column = std::string(buff);

//	printf("column : %s\n", &column[0]); fflush(stdout);

	// step over whitespace and '='
	while(*bufferPtr == ' ') bufferPtr++;
	if(*bufferPtr == '=')
	{
		bufferPtr++;
	}
	else
	{
		free(buff);
		printf("Invalid character."); fflush(stdout);
		return -1;
	}
	while(*bufferPtr == ' ') bufferPtr++;

	size = 0;
	while(*bufferPtr != ';')	// condition
	{
		if(size >= maxSize)
		{
			free(buff);
			printf("maxSize for where condition exceeded\n");
			return -1;
		}
		bufferPtr++;
		size++; 
	}
	memset(buff, 0, maxSize);
	strncpy(buff, bufferPtr-size, size);
	std::string condition = std::string(buff);

//	printf("condition : %s\n", &condition[0]); fflush(stdout);

	free(buff);

	if(!databases.size()) { printf("Could not UPDATE non-existent database"); fflush(stdout); return -1; }
	if(!databases[index].tables.size()) { printf("Could not UPDATE non-existent table"); fflush(stdout); return -1; }
	if(names.size() != values.size()) { printf("Number of names is not equal to number of values."); fflush(stdout); return -1; }
	
	int valueIndex = 0;

//	printf("checking database . . .\n"); fflush(stdout);

	// fine column and value index that needs updating . . .
	for(unsigned int i = 0; i < databases[index].tables.size(); i++)
	{
		// match table name
		if(!strcmp(&databases[index].tables[i].name[0], &table[0]))
		{
			bool isFound = false;
			for(unsigned int j = 0; j < databases[index].tables[i].columns.size(); ++j)
			{
				// match column name
				if(!strcmp(&databases[index].tables[i].columns[j].name[0], &column[0]))
				{
					isFound = true;
					for(unsigned int k = 0; k < databases[index].tables[i].columns[j].values.size(); ++k)
					{
						// match value name
						if(!strcmp(&databases[index].tables[i].columns[j].values[k][0], &condition[0]))
						{
							valueIndex = k;
							break;
						}
					}
					break;
				}
			}
			if(isFound)
			{
				for(unsigned int j = 0; j < databases[index].tables[i].columns.size(); ++j)
				{
					// match column name
					for(unsigned int x = 0; x < names.size(); ++x)
					{
						// update value
						if(!strcmp(&databases[index].tables[i].columns[j].name[0], &names[x][0]))
						{
							databases[index].tables[i].columns[j].values[valueIndex] = values[x];
							break;
						}
					}
				}
				break;
			}
			else
			{
				printf("Could not update table.\n");
				return -1;
			}
		}
	}
	return 0;
}

int
DatabaseServer::drop()
{
	return 0;
}

int
DatabaseServer::create()
{
	return 0;
}

void
DatabaseServer::printSelected()
{
	printf("\ndatabase[%d] : %s\n", index, &databases[index].name[0]);
	for(unsigned int j = 0; j < databases[index].tables.size(); ++j)
	{
		printf("\ttables[%d] : %s\n", j, &databases[index].tables[j].name[0]);
		for(unsigned int k = 0; k < databases[index].tables[j].columns.size(); ++k)
		{
			printf("\t\tcolumns[%d] : %s\n", k, &databases[index].tables[j].columns[k].name[0]);
			for(unsigned int l = 0; l < databases[index].tables[j].columns[k].values.size(); ++l)
			{
				printf("\t\t\tvalues[%d] : %s\n", k, &databases[index].tables[j].columns[k].values[l][0]);
			}
		}
	}
}

void
DatabaseServer::print()
{
	printf("\n");
	for(unsigned int i = 0; i < databases.size(); ++i)
	{
		printf("database[%d] : %s\n", i, &databases[i].name[0]);
		for(unsigned int j = 0; j < databases[i].tables.size(); ++j)
		{
			printf("\ttables[%d] : %s\n", j, &databases[i].tables[j].name[0]);
			for(unsigned int k = 0; k < databases[i].tables[j].columns.size(); ++k)
			{
				printf("\t\tcolumns[%d] : %s\n", k, &databases[i].tables[j].columns[k].name[0]);
				for(unsigned int l = 0; l < databases[i].tables[j].columns[k].values.size(); ++l)
				{
					printf("\t\t\tvalues[%d] : %s\n", k, &databases[i].tables[j].columns[k].values[l][0]);
				}
			}
		}
	}
}

bool
DatabaseServer::isLetter(char c)
{
	return ((tolower(c) <= 122) && (tolower(c) >= 97)) ? true : false; 
}

bool
DatabaseServer::isNumber(char c)
{
	return ((c <= 57) && (c >= 48)) ? true : false; 
}

bool
DatabaseServer::isString(char * string)
{
	if(isLetter(string[0])) return true;
	if(isDate(string)) return true;
	if(isIp(string)) return true;
	return false;
}

bool
DatabaseServer::isDate(char * string)
{
	if(strlen(string) < 3)
		return false;
	if(string[2] == ':')
		return true;
	return false;
}

bool
DatabaseServer::isIp(char * string)
{
	int dots = 0;
	for(unsigned int i = 0; i < strlen(string); ++i)
	{
		if(string[i] == '.')
		{
			dots += 1;
		}
	}
	return (dots > 1);
}