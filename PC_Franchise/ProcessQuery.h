#pragma once
// sprintf() �Լ��� �� ��, ����� ������ ���� ���ؼ� ���.
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>

using namespace std;
using namespace System;
using namespace System::Windows;
using namespace System::Windows::Forms;

class ProcessQuery
{
private:
	// hEnv �� ȯ�漳���� ���� �ڵ� ���� �����ϴ� ����.
	SQLHENV hEnv = NULL;

private:
	// hDbc �� ���ἳ���� ���� �ڵ� ���� �����ϴ� ����.
	/*
	* hDbc �ڵ��� hEnv �� �ʿ�� �ϸ�,
	* �ڿ� ���� hStmt �� hDbc �� �ʿ�� �ϱ� ������
	* ������ �ڵ� ���� �Ҵ����־�� ��.
	*/
	SQLHDBC hDbc = NULL;

public:
	ProcessQuery()
	{

	}

public:
	bool DBConnect()
	{
		SQLRETURN Ret;

		// SQLAllocHandle() hEnv �� ȯ�漳���� ���� �ڵ��� �Ҵ�.
		/*
		* SQLAllocHandle() �Լ��� ù��° ���ڴ� �ڵ��� Ÿ���� ����.
		* �ι�° ���ڴ� �Է¿� ���� �ڵ� ���� �ǹ�.
		* ���� �Է¹��� �ڵ� ���� �ǹ�.
		*/
		if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) != SQL_SUCCESS)
		{
			// �� �Լ��� �����ϸ�, false �� return.
			return false;
			// ���������� ����Ǹ�, ���� if ������ �Ѿ.
		}

		// SQLSetEnvAttr() SQL �� ȯ�溯���� ����.
		/*
		* SQLSetEnvAttr() �Լ��� ù��° ���ڴ� ȯ�� �ڵ��� ID.
		* �ι�° ���ڴ� �Ӽ� ��.
		* ����° ���ڴ� ODBC �� ���� �� �Ӽ�.
		* �׹�° ���ڴ� ����° ������ ũ��.
		* �� ������ ���� hEnv ȯ�� ������ ����.
		*/
		if (SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS)
		{
			return false;
		}

		// hDbc ������ connection �ڵ��� �Ҵ�.
		if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) != SQL_SUCCESS)
		{
			return false;
		}

		// SQLConnect() �Լ��� ���� ���� ���� ODBC �̸��� ���̵�, �н����带 �Է��Ͽ� SQL ������ ����.
		/*
		* SQLConnect() �Լ��� ù��° ���ڴ� ���� �ڵ��� ID.
		* �ι�° ���ڴ� ���� �̸�.
		* ����° ���ڴ� �ι�° ������ ũ��.
		* �׹�° ���ڴ� ���� ID.
		* �ټ���° ���ڴ� �׹�° ������ ũ��.
		* ������° ���ڴ� �н�����.
		* �ϰ���° ���ڴ� ������° ������ ũ��.
		*/
		Ret = SQLConnect(hDbc, (SQLCHAR*)"DB01_Server", SQL_NTS, (SQLCHAR*)"KIM", SQL_NTS, (SQLCHAR*)"1234", SQL_NTS);

		if ((Ret != SQL_SUCCESS) && (Ret != SQL_SUCCESS_WITH_INFO))
		{
			return false;
		}

		return true;
	}

public:
	void DBDisconnect()
	{
		SQLDisconnect(hDbc);					// SQL �������� ������ ����.
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc);	// �Ҵ�� �ڵ� hDbc �� hEnv �� ������ �°� ������.
		SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	}

public:
	SQLHENV gethEnv()
	{
		return hEnv;
	}

public:
	SQLHDBC gethDbc()
	{
		return hDbc;
	}

public:
	int SchemaCheck(char *table, string *name, short *len, short *type, unsigned int *size, short *isNullable)
	{
		if (DBConnect() == true) {
			printf("Connection Success!!\n");
			static SQLCHAR query[100];
			SQLHSTMT hStmt;
			SQLRETURN retcode;

			// - column �� ���� ������ ������ ������
			SQLCHAR ColumnName[30][30];			// column �� �̸�
			SQLSMALLINT ColumnNameLen[30];		// column �� �̸��� ũ��
			SQLSMALLINT ColumnDataType[30];		// column �� ������ Ÿ��(char, int)
			SQLULEN ColumnDataSize[30];			// column �� ������ ������
			SQLSMALLINT ColumnDataDigits[30];	// ...
			SQLSMALLINT ColumnDataNullable[30];
			// -

			SQLSMALLINT numCols = -1;

			if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {
				// SELECT ������
				sprintf((char*)query, "SELECT * FROM %s", table);
				printf("SELECT * FROM %s", table);
				// ��ȯ ���� ����.
				retcode = SQLExecDirect(hStmt, query, SQL_NTS);

				if (retcode == SQL_ERROR || retcode == SQL_NO_DATA_FOUND) {
					printf("Query Execute Error\n");
				}

				// �������� ���� ����� ������ column �� ������ numCols ������ ����.
				SQLNumResultCols(hStmt, &numCols);
				// column �� ������ŭ �ݺ�.
				for (int i = 0; i < numCols; i++) {
					// SQLDescribeCol() �Լ��� ����, column �� ������ ������ ����.
					SQLDescribeCol(
						hStmt,
						i + 1,
						ColumnName[i],
						30,
						&ColumnNameLen[i],
						&ColumnDataType[i],
						&ColumnDataSize[i],
						&ColumnDataDigits[i],
						&ColumnDataNullable[i]);

					// column �� ������ ���.
					name[i] = ((const char*)ColumnName[i]);
					len[i] = ColumnNameLen[i];
					type[i] = ColumnDataType[i];
					size[i] = ColumnDataSize[i];
					isNullable[i] = ColumnDataNullable[i];
					printf("\nColumn : %d\n", i + 1);
					printf("Column Name : %s\n Column Name Len : %d\n SQL Data Type : %d\n Data Size : %d\n",
						ColumnName[i], (int)ColumnNameLen[i], (int)ColumnDataType[i], (int)ColumnDataSize[i]);
				}

				SQLCloseCursor(hStmt);
				SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			}
			DBDisconnect();

			return numCols;
		}
		else {
			printf("Fail to Connect!!\n");
		}
	}

public:
	void CustomQuery(DataGridView^ Custom_dataGridView, string strQuery)
	{
		Custom_dataGridView->Rows->Clear();
		Custom_dataGridView->Columns->Clear();
		Custom_dataGridView->Refresh();

		if (DBConnect() == true) {
			printf("Connection Success!!\n");
			static SQLCHAR query[100];
			SQLHSTMT hStmt;
			SQLRETURN retcode;

			// - column �� ���� ������ ������ ������
			SQLCHAR ColumnName[30][30];			// column �� �̸�
			SQLSMALLINT ColumnNameLen[30];		// column �� �̸��� ũ��
			SQLSMALLINT ColumnDataType[30];		// column �� ������ Ÿ��(char, int)
			SQLULEN ColumnDataSize[30];			// column �� ������ ������
			SQLSMALLINT ColumnDataDigits[30];	// ...
			SQLSMALLINT ColumnDataNullable[30];
			// -

			SQLSMALLINT numCols = -1;

			char char_query[100];

			string strData;
			string* attName;
			String^ StrColumnName;
			String^ StrData;

			SQLCHAR columnData[20][20];
			SQLLEN nulldata[20];

			strcpy((char*)query, strQuery.c_str());

			if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {
				// SELECT ������
				sprintf((char*)query, "%s", query);
				// ��ȯ ���� ����.
				retcode = SQLExecDirect(hStmt, query, SQL_NTS);

				if (retcode == SQL_ERROR || retcode == SQL_NO_DATA_FOUND) {
					printf("Query Execute Error\n");
				}
				// �������� ���� ����� ������ column �� ������ numCols ������ ����.
				SQLNumResultCols(hStmt, &numCols);
				if (numCols > 0)
				{
					attName = new string[numCols];
					// column �� ������ŭ �ݺ�.
					for (int i = 0; i < numCols; i++) {
						// SQLDescribeCol() �Լ��� ����, column �� ������ ������ ����.
						SQLDescribeCol(
							hStmt,
							i + 1,
							ColumnName[i],
							30,
							&ColumnNameLen[i],
							&ColumnDataType[i],
							&ColumnDataSize[i],
							&ColumnDataDigits[i],
							&ColumnDataNullable[i]);

						// column �� ������ ���.
						attName[i] = ((const char*)ColumnName[i]);
						printf("\nColumn : %d\n", i + 1);
						printf("Column Name : %s\n Column Name Len : %d\n SQL Data Type : %d\n Data Size : %d\n",
							ColumnName[i], (int)ColumnNameLen[i], (int)ColumnDataType[i], (int)ColumnDataSize[i]);
					}

					Custom_dataGridView->ColumnCount = numCols;
					for (int i = 0; i < numCols; i++)
					{
						StrColumnName = gcnew String(attName[i].c_str());
						Custom_dataGridView->Columns[i]->Name = StrColumnName;
					}

					for (int i = 0; i < numCols; i++)
					{
						SQLBindCol(hStmt, i + 1, SQL_C_CHAR, columnData[i], 20, &nulldata[i]);
					}

					int j = 0;
					while (SQLFetch(hStmt) != SQL_NO_DATA) {
						Custom_dataGridView->Rows->Add();
						for (int i = 0; i < numCols; i++)
						{
							if (nulldata[i] == SQL_NULL_DATA)
								StrData = gcnew String("NULL");
							else
							{
								strData = ((const char*)columnData[i]);
								StrData = gcnew String(strData.c_str());
							}
							Custom_dataGridView->Rows[j]->Cells[i]->Value = StrData;
						}
						j++;
					}

					delete[] attName;
				}

				SQLCloseCursor(hStmt);
				SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			}
			DBDisconnect();
		}
		else {
			printf("Fail to Connect!!\n");
		}
	}

	void UpdateQuery(string strQuery)
	{
		if (DBConnect() == true) {
			printf("Connection Success!!\n");
			static SQLCHAR query[100];
			SQLHSTMT hStmt;
			SQLRETURN retcode;

			char char_query[100];

			strcpy((char*)query, strQuery.c_str());

			if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {
				// SELECT ������
				sprintf((char*)query, "%s", query);
				// ��ȯ ���� ����.
				retcode = SQLExecDirect(hStmt, query, SQL_NTS);

				if (retcode == SQL_ERROR || retcode == SQL_NO_DATA_FOUND) {
					printf("Query Execute Error\n");
				}

				SQLCloseCursor(hStmt);
				SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			}
			DBDisconnect();
		}
		else {
			printf("Fail to Connect!!\n");
		}
	}

	void OutputQuery(string* output, string strQuery)
	{
		if (DBConnect() == true) {
			printf("Connection Success!!\n");
			static SQLCHAR query[100];
			SQLHSTMT hStmt;
			SQLRETURN retcode;

			SQLSMALLINT numCols = -1;

			SQLCHAR columnData[20][20];
			SQLLEN nulldata[20];

			string strData;

			char char_query[100];

			strcpy((char*)query, strQuery.c_str());

			if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {
				// SELECT ������
				sprintf((char*)query, "%s", query);
				// ��ȯ ���� ����.
				retcode = SQLExecDirect(hStmt, query, SQL_NTS);

				if (retcode == SQL_ERROR || retcode == SQL_NO_DATA_FOUND) {
					printf("Query Execute Error\n");
				}

				SQLNumResultCols(hStmt, &numCols);
				if (numCols > 0)
				{
					for (int i = 0; i < numCols; i++)
					{
						SQLBindCol(hStmt, i + 1, SQL_C_CHAR, columnData[i], 20, &nulldata[i]);
					}
				}

				int j = 0;
				while (SQLFetch(hStmt) != SQL_NO_DATA) {
					for (int i = 0; i < numCols; i++)
					{
						if (nulldata[i] == SQL_NULL_DATA)
							output[i] = "NULL";
						else
						{
							strData = ((const char*)columnData[i]);
							output[i] = strData;
							printf("%s %d\n", output[i], i);
						}
					}
					j++;
				}

				SQLCloseCursor(hStmt);
				SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			}
			DBDisconnect();
		}
		else {
			printf("Fail to Connect!!\n");
		}
	}
};