#include <mysql/mysql.h>
#include "./sqlsnake.structs.h"

class SqlSnake{
	private:
		unsigned int port = 3306;
                string server = "";
                string user = "";
                string password = "";
                string database = "";
                bool inited = false;
                bool dbSelected = false;
                bool dbConnected = false;

                int rowCount = 0;
                int fieldCount = 0;
                int affectedRows = 0;

                string warning = "";
                string error = "";

                MYSQL *connection = NULL;
                MYSQL_RES *results = NULL;

		///
		// Sanitization must be done prior to using this function.
		///
		string generateWhereString(sqlwherelist_t wheres){
			string ret = "";

			if(wheres.wheres == NULL || wheres.whereCount <= 0){
				error = "Failed to generate where string, no wheres provided.";
				return "";
			}

			if((wheres.whereCount - wheres.seperatorCount) != 1){
				error = "Where statement seperators are improperly configured.";
				return "";
			}

			for(int i=0; i<wheres.whereCount; i++){
				ret += wheres.wheres[i].column + wheres.wheres[i].operation + wheres.wheres[i].value;
				if((i) < wheres.seperatorCount){
					ret += " "+wheres.seperators[i] + " ";
				}
			}
			return ret;
		}
	public:
		sqlresults_t result;
		string getError(){
                        return this->error;
                }

                bool dbIsSelected(){
                        return this->dbSelected;
                }

                bool hasError(){
                        if(this->error != "")
                                return true;
                        return false;
                }

		string getDatabase(void){
			return database;
		}

		string sanitize(string dirty){
                        if(!this->connect() || !this->dbConnected){
                                error = "Failed to connect to db service for sanitization.";
				return "";
			}

                        char *to = new char[dirty.length()]; memset(to, '\0', dirty.length());

                        mysql_real_escape_string_quote(this->connection, to, dirty.c_str(), dirty.length(), '\'');
                        dirty = to;
                        to = new char[dirty.length()]; memset(to, '\0', dirty.length());

                        mysql_real_escape_string_quote(this->connection, to, dirty.c_str(), dirty.length(), '"');
                        dirty = to;
                        to = new char[dirty.length()]; memset(to, '\0', dirty.length());

                        mysql_real_escape_string_quote(this->connection, to, dirty.c_str(), dirty.length(), '.');
                        string ret = to;

                        this->close();
                        return ret;
                }

		void close(){
                        if(this->results != NULL){
                                mysql_free_result(this->results);
                                this->results = NULL;
                        }
                        if(this->connection != NULL){
                                mysql_close(this->connection);
                                this->connection = NULL;
                        }
                        this->dbConnected = false;
                        this->dbSelected = false;
                        //this->error = "";
                        //this->warning = "";
                }

                bool newQuery(string q){
                        if(this->dbConnected == false || this->connection == NULL){
                                if(!this->connect()){
                                        return false;
                                }
                        }
                        if(this->results != NULL){
                                mysql_free_result(this->results);
                                this->results = NULL;
                        }
                        this->error = "";
                        this->warning = "";

                        return query(q);
                }

		bool query(string q){
                        if(!this->connect()){
                                return false;
                        }
                        if(mysql_query(this->connection, q.c_str())){
                                this->error = mysql_error(this->connection);
                                return false;
                        }
                        this->results = mysql_store_result(this->connection);
                        this->affectedRows = mysql_affected_rows(this->connection);
                        this->fieldCount = mysql_field_count(this->connection);

                        if(this->fieldCount > 0){
                                this->rowCount = mysql_num_rows(this->results);
                        }else{
                                this->rowCount = -1;
                        }
                        return true;
                }

               	//MYSQL_RES *getResults(){
                //        return this->results;
                //}

                MYSQL_ROW getRow(){
                        if(this->fieldCount > 0)
                                return mysql_fetch_row(this->results);
                        else
                                return NULL;
                }

		size_t getFieldCount(void){
			size_t ret = fieldCount;
			return ret;
		}
		string *getFields(void){
			if(fieldCount <= 0){
				return NULL;
			}
			string * ret = NULL;
			size_t size = fieldCount;
			ret = new string[size];
			MYSQL_FIELD *fields = mysql_fetch_fields(results);
			for(int i=0; i<size; i++){
				ret[i] = fields[i].name;
			}
			return ret;
		}


		sqlresults_t getResults(void){
			sqlresults_t ret;
			ret.resultCount = rowCount;
			ret.fieldCount = fieldCount;
			ret.fields = getFields();
			ret.results = new sqlresult_t[ret.resultCount];

			MYSQL_ROW row;
			int ctr = 0;
			while((row = getRow()) != NULL){
				ret.results[ctr].valueCount = fieldCount;
				for(int i=0; i<fieldCount; i++){
					ret.results[ctr].values[i] = row[i];
				}
				ctr++;
			}
			
			return ret;
		}

		
                void printResults(){
                        MYSQL_ROW row;
                        printf("Row Count : %d\n", this->rowCount);
                        printf("Affected Rows : %d\n", this->affectedRows);
                        printf("Field Count : %d\n", this->fieldCount);
                        while((row = this->getRow()) != NULL){
                                for(int i=0; i<fieldCount; i++){
                                        printf("%s |", row[i]);
                                }printf("\n");
                        }
                }

		bool connect(){
                        return this->init(this->server, this->port, this->user, this->password, this->database);
                }

		bool init(string server, unsigned int port, string user, string password, string database){
                        this->error = "";
                        this->warning = "";
                        /*
                         * Error checking
                         * */
                        if(server == ""){
                                this->error = "init() : Provide a server IP.";
                                return false;
                        }
                        this->server = server;

                        if(port > 65535){
                                this->error = "init() : Invalid port number.";
                                return false;
                        }
                        this->port = port;

                        if(user == ""){
                                this->error = "init() : Provide a server username.";
                                return false;
                        }
                        this->user = user;

                        if(password == ""){
                                this->error = "init() : Provide a server password";
                                return false;
                        }
                        this->password = password;

                        if(database == ""){
                                this->warning = "init() : no database provided.";
                        }
                        this->database = database;

			/*
                         * DB Initalization.
                         * */
                        this->connection = mysql_init(NULL);
                        if(!mysql_real_connect(this->connection, this->server.c_str(), this->user.c_str(), this->password.c_str(), this->database.c_str(), this->port, NULL, 0)){
                                this->error = mysql_error(this->connection);
                                string check = "Unknown database '" + this->database + "'";
                                if(this->error == check){
                                        this->connection = mysql_init(NULL);
                                        if(!mysql_real_connect(this->connection, this->server.c_str(), this->user.c_str(), this->password.c_str(), "", this->port, NULL, 0)){
                                                this->error = mysql_error(this->connection);
                                                this->inited = false;
                                                this->dbSelected = false;
                                                return false;
                                        }
                                        this->warning = "No Database Selected.";
                                        this->dbSelected = false;
                                        this->error = "";
                                        this->inited = true;
                                        this->dbConnected = true;;
                                        return true;
                                }
                                return false;
                        }
                        this->dbSelected = true;
                        this->inited = true;
                        this->dbConnected = true;
                        return true;
                }

		bool useDatabase(string dbName){
			error = "";
			dbSelected = false;
			database = "";
                        dbName = sanitize(dbName);
                        if(hasError()){
                                error = "failed to select database\n"+error;
                                return false;
                        }
			
			string q = "USE "+ dbName + ";";
			if(!newQuery(q)){
				return false;
			}else{
				dbSelected = true;	
				database = dbName;
				return true;
			}
		}

		bool createDatabase(string dbName){
			error = "";
			dbName = sanitize(dbName);
			if(hasError()){
				error = "failed to create database\n"+error;
				return false;
			}
			
			string q = "CREATE DATABASE IF NOT EXISTS "+dbName + ";";
			return newQuery(q);
		}

		bool removeDatabase(string dbName){
			error = "";
			dbName = sanitize(dbName);
                        if(hasError()){
                                error = "failed to remove database\n"+error;
                                return false;
                        }

			string q = "DROP DATABASE " + dbName + ";";
			return newQuery(q);
		}

		sqlcolumn_t generateColumn(string name, string type, string options){
			sqlcolumn_t ret;
			ret.name = name;
			ret.type = type;
			ret.options = options;
			ret.primary = false;
			ret.foreign = false;
			return ret;
		}

		sqlcolumn_t generatePrimaryColumn(string name, string type, string options){
			sqlcolumn_t ret;
                        ret.name = name;
                        ret.type = type;
                        ret.options = options;
                        ret.primary = true;
                        ret.foreign = false;
                        return ret;
		}

		sqlcolumn_t generateForeignColumn(string name, string type, string options, string foreignTable, string foreignColumn){
			sqlcolumn_t ret;
                        ret.name = name;
                        ret.type = type;
                        ret.options = options;
                        ret.primary = false;
                        ret.foreign = true;
			ret.foreignTable = foreignTable;
			ret.foreignColumn = foreignColumn;
                        return ret;
		}

		bool deleteTable(string tableName){
			error = "";
			if(tableName == ""){
				error = "No table provided to delete.";
				return false;
			}

			string q = "DROP TABLE "+sanitize(tableName) + ";";
			return newQuery(q);
		}
		bool createTable(sqltable_t table){
			error = "";
			if(table.name == ""){
				error = "No table name provided.";
				return false;
			}
			if(table.colCount <= 0){
				error = "No columns provided for table.";
				return false;
			}

			string q = "CREATE TABLE "+sanitize(table.name)+"( ";
			
			// Check for primary key
			string primary = "";
			bool hasPrimary = false;
			for(int i=0; i<table.colCount; i++){
				if(table.cols[i].primary){
					if(hasPrimary){
						error = "Table can only have 1 primary key.";
						return false;
					}
					hasPrimary = true;
					primary = "PRIMARY KEY ("+sanitize(table.cols[i].name)+")";
					break;
				}
			}
			
			// Check for foreign keys
			string *foreign = NULL;
			size_t foreignCount = 0;
			for(int i=0; i<table.colCount; i++){
				if(table.cols[i].primary && table.cols[i].foreign){
					error = "Column cannot be both forign and primary key.";
					return false;
				}
				if(table.cols[i].foreign)
					foreignCount++;
			}

			if(foreignCount>0){
				foreign = new string[foreignCount];
				int ctr = 0;
				for(int i=0; i<table.colCount; i++){
					if(table.cols[i].foreign){
						foreign[ctr] = "FOREIGN KEY ("+sanitize(table.cols[i].name);
						foreign[ctr] += ") REFEREBCES "+sanitize(table.cols[i].foreignTable);
						foreign[ctr] += "("+sanitize(table.cols[i].foreignColumn) + ")";
						ctr++;
					}
				}
			}

			// Craft the query
			for(int i=0; i<table.colCount; i++){
				q += table.cols[i].name + " "+table.cols[i].type;
				if(table.cols[i].options != "")
					q+= " "+table.cols[i].options;
				if(hasPrimary || foreignCount > 0 || i+1 != table.colCount){
					q+= ",";
				}
			}
			// Add primary key to query
			if(hasPrimary){
				q += primary;
				if(foreignCount>0)
					q += ",";
			}

			// add foreign keys
			if(foreignCount > 0){
				for(int i=0; i<foreignCount; i++){
					q += foreign[i];
					if(i+1 != foreignCount){
						q += ",";
					}
				}
				delete[] foreign;
			}
			
			q += ");";

			return newQuery(q);
		}

		bool secureInsert(sqlinsert_t ins){	
			error = "";
			if(ins.table == ""){
				error = "No table name provided to insert query.";
				return false;
			}
			if(ins.count <= 0){
				error = "No values or columns provided.";
				return false;
			}

			if(ins.values == NULL){
				error = "No values provided.";
				return false;
			}

			if(ins.cols == NULL){
				error = "No columns provided.";
				return false;
			}
			string q = "INSERT INTO " + ins.table + "(";
			// add columns to query
			for(int i=0; i<ins.count; i++){
				q += sanitize(ins.cols[i]);
				if(i+1 != ins.count)
					q += ",";
			}
			
			q += ") VALUES (";
			// add sanitized values
			for(int i=0; i<ins.count; i++){
				q += "'"+sanitize(ins.values[i])+"'";
				if(i+1 != ins.count)
					q += ",";
			}
			
			q += ");";
			return newQuery(q);
		}

		sqlwhere_t generateWhere(string column, string operation, string value, bool quoted){
                        sqlwhere_t ret;
                        ret.column = sanitize(column);
                        ret.operation = operation;
                        if(quoted)
                                ret.value = "'"+sanitize(value)+"'";
                        else
                                ret.value = sanitize(value);
                        return ret;
                }
		sqlwhere_t generateWhere(string column, string operation, string value){
			sqlwhere_t ret;
			ret.column = sanitize(column);
			ret.operation = operation;
			ret.value = sanitize(value);
			return ret;
		}

		sqlwherelist_t addToWhere(sqlwherelist_t wherelist, sqlwhere_t where, string seperator){
			if(wherelist.whereCount <= 0 || wherelist.wheres == NULL){
				if(wherelist.wheres != NULL)
					delete[] wherelist.wheres;
				wherelist.wheres = new sqlwhere_t[1];
				wherelist.whereCount = 1;
				wherelist.wheres[0] = where;
				if(wherelist.seperators != NULL){
					delete[] wherelist.seperators;
					wherelist.seperators = NULL;
				}
				wherelist.seperatorCount = 0;
				return wherelist;
			}

			sqlwhere_t *oldWheres = new sqlwhere_t[wherelist.whereCount];
			for(int i=0; i<wherelist.whereCount; i++){
				oldWheres[i] = wherelist.wheres[i];
			}
			wherelist.whereCount++;
			delete[] wherelist.wheres;
			wherelist.wheres = new sqlwhere_t[wherelist.whereCount];

			for(int i=0; i<wherelist.whereCount-1; i++){
				wherelist.wheres[i] = oldWheres[i];
			}
			wherelist.wheres[wherelist.whereCount-1] = where;

			if(seperator != ""){
				if(wherelist.seperatorCount <= 0 || wherelist.seperators == NULL){
					if(wherelist.seperators != NULL){
						delete[] wherelist.seperators;
					}

					wherelist.seperators = new string[1];
					wherelist.seperatorCount = 1;
					wherelist.seperators[0] = seperator;
				}else{
					string *oldSeperators = new string[wherelist.seperatorCount];
					for(int i=0; i<wherelist.seperatorCount; i++){
						oldSeperators[i] = wherelist.seperators[i];
					}

					delete[] wherelist.seperators;
					wherelist.seperatorCount++;
					wherelist.seperators = new string[wherelist.seperatorCount];
					for(int i=0; i<wherelist.seperatorCount-1; i++){
						wherelist.seperators[i] = oldSeperators[i];
					}
					wherelist.seperators[wherelist.seperatorCount-1] = seperator;
					delete[] oldSeperators;

				}
			}

			delete[] oldWheres;
			return wherelist;
		}

		bool secureSelect(sqlselect_t sel){
			error = "";
			if(sel.table == ""){
				error = "No table provide to select statement.";
				return false;
			}
			if(sel.cols == NULL || sel.colCount <= 0){
				error = "No select targets provided.";
				return false;
			}
			string q = "SELECT ";
			for(int i=0; i<sel.colCount; i++){
				q += sanitize(sel.cols[i]);
				if(i+1 != sel.colCount){
					q += ",";
				}
			}

			q += " FROM " + sanitize(sel.table);
			if(sel.hasWhere){
				q += " WHERE " + generateWhereString(sel.wheres);
			}
			q += ";";

			return newQuery(q);
		}

		bool secureUpdate(sqlupdate_t update){
			error = "";
			if(update.table == ""){
				error = "No table provided to update statement.";
				return false;
			}

			if(update.cols == NULL){
				error = "No target columns provided to update statement.";
				return false;
			}

			if(update.values == NULL){
				error = "No replacement values provided to update statement.";
				return false;
			}

			if(update.valueCount <= 0){
				error = "0 values and target columns provided.";
				return false;
			}

			string q = "UPDATE TABLE "+sanitize(update.table) + " SET ";
			for(int i=0; i<update.valueCount; i++){
				q += sanitize(update.cols[i]) + "=" + sanitize(update.values[i]);
				if((i+1) < update.valueCount){
					q += ",";
				}
			}

			if(update.wheres.whereCount > 0){
				q += " WHERE " + generateWhereString(update.wheres);
			}
			return true;
		}



		bool secureDelete(string tableName, sqlwherelist_t wheres){
			error = "";
			if(tableName == ""){
				error = "No table provided for row deletion.\n";
				return false;
			}
			string q = "DELETE FROM " + sanitize(tableName) + " WHERE "+generateWhereString(wheres)+";";
		
			return newQuery(q);
		}
};
