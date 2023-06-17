#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>

using namespace std;
#include "./sqlSnake.class.h"

SqlSnake testInit(SqlSnake sql, string server, unsigned int port, string user, string password, string database){
	if(!sql.init(server, port, user, password, database)){
                printf("Failed to init mysql connection.\n\t%s\n", sql.getError().c_str());
                exit(EXIT_FAILURE);
        }else{
                printf("[+] Sql Class successfully initalized.\n");
        }
	return sql;
}

SqlSnake testDbCreate(SqlSnake sql, string database){
	if(!sql.createDatabase(database)){
                printf("Failed to create database.\n\t%s\n", sql.getError().c_str());
                sql.close();
                exit(EXIT_FAILURE);
        }else{
                printf("[+] Test DB Created.\n");
        }
	return sql;
}

SqlSnake testUseDb(SqlSnake sql, string database){
	if(!sql.useDatabase(database)){
                printf("Failed to select database.\n\t%s\n", sql.getError().c_str());
                sql.close();
                exit(EXIT_FAILURE);
        }else{
                printf("[+] Databse selected successfully\n");
        }
	return sql;
}

SqlSnake testTableCreation(SqlSnake sql){
	sqltable_t table;

        table.name = "test_table";
        table.cols = new sqlcolumn_t[3];
        table.colCount = 3;
	table.cols[0] = sql.generatePrimaryColumn("test_id", "int", "NOT NULL AUTO_INCREMENT");
	table.cols[1] = sql.generateColumn("test_firstname", "varchar(50)", "NOT NULL");
	table.cols[2] = sql.generateColumn("test_lastname", "varchar(50)", "NOT NULL");

	if(!sql.createTable(table)){
                printf("Failed to create table.\n\t%s\n", sql.getError().c_str());
                if(!sql.removeDatabase(sql.getDatabase())){
                        printf("Failed to delete database.\n\t%s\n", sql.getError().c_str());
                }
                sql.close();
                exit(EXIT_FAILURE);

        }else{
                printf("[+] Created table with primary key\n");
        }
	return sql;
}

SqlSnake testTableInsert(SqlSnake sql){
	sqlinsert_t ins;
        ins.table = "test_table";
        ins.count = 2;
	ins.values = new string[2];
        ins.cols = new string[2];
	ins.cols[0] = "test_firstname";
	ins.cols[1] = "test_lastname";
	ins.values[0] = "Morning";
	ins.values[1] = "Star";

	if(!sql.secureInsert(ins)){
                printf("Failed to insert into table.\n\t%s\n", sql.getError().c_str());
		if(!sql.removeDatabase(sql.getDatabase())){
                        printf("Failed to delete database.\n\t%s\n", sql.getError().c_str());
                }
                sql.close();
                exit(EXIT_FAILURE);
        }else{
                printf("[+] Insert Successful!\n");
                ins.values[1] = "Moon";
                sql.secureInsert(ins);
                ins.values[1] = "Sun";
                sql.secureInsert(ins);
                ins.values[1] = "WAKAFLAKA";
                sql.secureInsert(ins);
        }
	return sql;
}

SqlSnake testTableSelect(SqlSnake sql){
	sqlselect_t select;
        select.table = "test_table";
        select.cols = new string[2];
        select.colCount = 2;
        select.cols[0] = "test_firstname";
	select.cols[1] = "test_lastname";
        select.hasWhere = true;
        select.wheres = sql.addToWhere(select.wheres,
                                        sql.generateWhere("test_id", "=", "2"),
                                        "");
        select.wheres = sql.addToWhere(select.wheres,
                                        sql.generateWhere("test_id", "=", "1"),
                                        "OR");
	if(!sql.secureSelect(select)){
                printf("Failed to run select statement. : %s\n", sql.getError().c_str());
                if(!sql.removeDatabase(sql.getDatabase())){
                        printf("Failed to delete database.\n\t%s\n", sql.getError().c_str());
                }
                sql.close();
                exit(EXIT_FAILURE);
        }else{
		printf("\n\n");
                sqlresults_t test = sql.getResults();
                printf("ResultCount : %ld\nField Count : %ld\n\n", test.resultCount, test.fieldCount);
                for(int i=0; i<test.fieldCount; i++){
                        printf("%s\t|", test.fields[i].c_str());
                }printf("\n");
                for(int i=0; i<test.resultCount; i++){
                        for(int j=0; j<test.fieldCount; j++){
                                printf("%s\t|", test.results[i].values[j].c_str());
                        }printf("\n");
                }

                printf("[+] Select Query Successful!\n");
        }
	return sql;
}

SqlSnake testRowDelete(SqlSnake sql){
	sqlwherelist_t whereTest;
        whereTest = sql.addToWhere(whereTest,
                                sql.generateWhere("test_id", "=", "2"), "");

	if(!sql.secureDelete("test_table", whereTest)){
                printf("Failed to run DELETE statement. : %s\n", sql.getError().c_str());
                if(!sql.removeDatabase(sql.getDatabase())){
                        printf("Failed to delete database.\n\t%s\n", sql.getError().c_str());
                }
                sql.close();
                exit(EXIT_FAILURE);
        }else{
		printf("[+] Successfully deleted record!\n");
	}

	return sql;
}

SqlSnake testDatabaseDelete(SqlSnake sql){
	if(!sql.removeDatabase(sql.getDatabase())){
                printf("Failed to delete database.\n\t%s\n", sql.getError().c_str());
                sql.close();
                exit(EXIT_FAILURE);
        }else{
                printf("[+] Successfully deleted test database.\n");
        }
	return sql;
}

int main(){
	SqlSnake sql;

	string server;
	printf("Enter a server hostname / ip > ");cin >> server;
	
	unsigned int port;
	printf("Enter a server port number (default is typically 3306) > ");cin >> port;

	string user;
	printf("Enter your mysql user name > ");cin >> user;

	string password;
	printf("Enter your mysql password > ");cin>>password;

	string database;
	printf("Enter an unused database name to test with > ");cin>>database;

	sql = testInit(sql, server, port, user, password, database);
	sql = testDbCreate(sql, database);
	sql = testUseDb(sql, database);
	sql = testTableCreation(sql);
	sql = testTableInsert(sql);
	sql = testTableSelect(sql);
	sql = testRowDelete(sql);
	sql = testDatabaseDelete(sql);

	sql.close();
	exit(EXIT_SUCCESS);
}
