#! python3
# database.py

import MySQLdb, logging, sys
logging.basicConfig(stream=sys.stderr, level=logging.ERROR)

#Class for database objects. 
class Database:
	
	# initalizes with connection to database
	def __init__(self, database):
		self.db = MySQLdb.connect(
				    user="root",
				    db=database)

	# queries database using the given sql statement and values/
	# This is properly escaped to curb sql injection. Values should be a
	# tuple
	def query(self, sql_statement, values):
		cursor = self.db.cursor()
		try:
			cursor.execute(sql_statement, values)

			#returns the first row of values
			values = cursor.fetchone()
		except MySQLdb.Error as e:
			logging.error("{}".format(e))
			values = -1

		#close the cursor
		cursor.close()
		return values

	# inserts values into the db using the given sql statement and values.
	# This is properly escaped to curb sql injection. Values should be a
	# tuple
	def insert(self, sql_statement, values):
		cursor = self.db.cursor()
		try: 
			cursor.execute(sql_statement, values)	
		except MySQLdb.Error as e:
			logging.error("{}".format(e))

		self.db.commit()
		cursor.close()

	# closes connection to db when called 
	def close(self):
		self.db.close()

