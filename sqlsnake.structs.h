struct sqlsnake_column{
        string name = "";
        string type = "";
        string options = "";
        string foreignTable = "";
        string foreignColumn = "";
        bool primary = false;
        bool foreign = false;
};
typedef struct sqlsnake_column sqlcolumn_t;

struct sqlsnake_table{
        string name = "";
        size_t colCount = 0;
        sqlcolumn_t *cols = NULL;
};
typedef struct sqlsnake_table sqltable_t;

struct sqlsnake_secure_insert{
        string table = "";
        size_t count = 0;
        string *values = NULL;
        string *cols = NULL;
};
typedef struct sqlsnake_secure_insert sqlinsert_t;

struct sqlsnake_secure_where{
        string column = "";
        string operation = "";
        string value = "";
};
typedef struct sqlsnake_secure_where sqlwhere_t;

struct sqlsnake_secure_where_list{
        sqlwhere_t *wheres = NULL;
        size_t whereCount = 0;
        string *seperators = NULL;
        size_t seperatorCount = 0;
};
typedef struct sqlsnake_secure_where_list sqlwherelist_t;

struct sqlsnake_secure_select{
        string table = "";
        string *cols = NULL;
        size_t colCount = 0;
        bool hasWhere = false;
        sqlwherelist_t wheres;

};
typedef struct sqlsnake_secure_select sqlselect_t;

struct sqlsnake_result{
        const int valueMax = 100;
        size_t valueCount = 0;
        string values[100];
};
typedef struct sqlsnake_result sqlresult_t;

struct sqlsnake_results{
        size_t fieldCount = 0;
        size_t resultCount = 0;
        size_t affectedCount = 0;
        string *fields = NULL;
        sqlresult_t *results = NULL;
};
typedef struct sqlsnake_results sqlresults_t;

struct sqlsnake_secure_update{
	string table = "";
	string *cols = NULL;
	string *values = NULL;
	size_t valueCount = 0;
	sqlwherelist_t wheres;
};
typedef struct sqlsnake_secure_update sqlupdate_t;
