#ifndef _DMySQLDataProvider_
#define _DMySQLDataProvider_

#ifdef WIN32
#include <winsock.h>
#endif
#include <mysql.h>
#include <vector>
#include <map>

#include "CCDB/Providers/DataProvider.h"
#include "CCDB/Providers/MySQLConnectionInfo.h"
#include "CCDB/Model/ConstantsTypeTable.h"

#define CCDB_DEFAULT_MYSQL_USERNAME  "ccdbuser"
#define CCDB_DEFAULT_MYSQL_PASSWORD  ""
#define CCDB_DEFAULT_MYSQL_URL 		 "localhost"
#define CCDB_DEFAULT_MYSQL_PORT 	 3306
#define CCDB_DEFAULT_MYSQL_DATABASE	 "ccdb"

///We making this define to be sure if we switch to other library nothing will change
#define MYSQL_ULONG my_ulonglong

using namespace std;

namespace ccdb
{

class MySQLDataProvider: public DataProvider
{
	
public:
	MySQLDataProvider(void);
	virtual ~MySQLDataProvider(void);
	
	virtual bool IsStoredObjectsOwner();
	virtual void IsStoredObjectsOwner(bool flag);
	
	//----------------------------------------------------------------------------------------
	//	C O N N E C T I O N
	//----------------------------------------------------------------------------------------
		
	/**
	 * @brief Connects to database using connection string
	 * 
	 * Connects to database using connection string
	 * connection string might be in form: 
	 * mysql://<username>:<password>@<mysql.address>:<port> <database>
	 * 
	 * @param connectionString "mysql://<username>:<password>@<mysql.address>:<port> <database>"
	 * @return true if connected
	 */
	virtual bool Connect(string connectionString);
	
	/**
	 * @brief Connects to database using DMySQLConnectionInfo
	 * 
	 * @param connection 
	 * @return true if connected
	 */
	virtual bool Connect(MySQLConnectionInfo connection);
	
	/**
	 * @brief indicates ether the connection is open or not
	 * 
	 * @return true if  connection is open
	 */
	virtual bool IsConnected();
	
	/**
	 * @brief closes connection to data
	 */
	virtual void Disconnect();

	/** @brief Parse Connection String
	 *
	 * @param   [in]  conStr
	 * @param   [out] DMySQLConnectionInfo & connection
	 * @return   bool
	 */
	static bool ParseConnectionString(std::string conStr, MySQLConnectionInfo &connection);

	/** @brief Checks Connection and report error if not connected
	 *
	 * This function checks if the provider is connected now and return true if it is
	 * If it is not connected it sets error. This functions must be used in places where 
	 * connection must be and is need to be checked.
	 * The difference with IsConnected() is that this function does report error, 
	 * while IsConnected() is only indicates this class connected or not
	 *
	 * @param error source - the source where this check is performed to be printed in error
	 * @return  true if connected 
	 */
	virtual bool CheckConnection(const string& errorSource="");

	//----------------------------------------------------------------------------------------
	//	D I R E C T O R Y   M A N G E M E N T
	//----------------------------------------------------------------------------------------
	/** @brief Gets directory by its full path
	*
	* @param   Full path of the directory
	* @return  DDirectory object if directory exists, NULL otherwise
	*/
	virtual Directory* GetDirectory(const string& path);
	
	/** @brief return reference to root directory
	 * 
	 * @warning User should not delete this object 
	 *
	 * @return    DDirectory object pointer
	 */
	virtual Directory * const GetRootDirectory();

	/** @brief Searches for directory 
	 *
	 * Searches directories that matches the pattern 
	 * inside parent directory with path @see parentPath 
	 * or globally through all type tables if parentPath is empty
	 * The pattern might contain
	 * '*' - any character sequence 
	 * '?' - any single character
	 *
	 * paging could be done with @see startWith  @see take
	 * startRecord=0 and selectRecords=0 means select all records;
	 *
	 * objects that are contained in vector<DDirectory *>& resultDirectories will be
	 * 1) if this provider owned - deleted (@see ReleaseOwnership)
	 * 2) if not owned - just leaved on user control
	 *
	 * @param  [out] resultDirectories	search result
	 * @param  [in]  searchPattern		Pattern to search
	 * @param  [in]  parentPath			Parent path. If NULL search through all directories
	 * @param  [in]  startRecord		record number to start with
	 * @param  [in]  selectRecords		number of records to select. 0 means select all records
	 * @return bool true if there were error (even if 0 directories found) 
	 */
	virtual bool SearchDirectories(vector<Directory *>& resultDirectories, const string& searchPattern, const string& parentPath="", int take=0, int startWith=0);
	
	/** @brief SearchDirectories
	 *
	 * Searches directories that matches the pattern 
	 * inside parent directory with path @see parentPath 
	 * or globally through all type tables if parentPath is empty
	 * The pattern might contain
	 * '*' - any character sequence 
	 * '?' - any single character
	 *
	 * paging could be done with @see startWith  @see take
	 * startRecord=0 and selectRecords=0 means select all records;
	 *
	 * objects that are contained in vector<DDirectory *>& resultDirectories will be
	 * 1) if this provider owned - deleted (@see ReleaseOwnership)
	 * 2) if not owned - just leaved on user control
	 *
	 * @param  [in]  searchPattern		Pattern to search
	 * @param  [in]  parentPath			Parent path. If NULL search through all directories
	 * @param  [in]  startRecord		record number to start with
	 * @param  [in]  selectRecords		number of records to select. 0 means select all records
	 * @return list of 
	 */
	virtual vector<Directory *> SearchDirectories(const string& searchPattern, const string& parentPath="", int take=0, int startWith=0);

	/** @brief Creates directory using parent parentDirFullPath
	 * 
	 * @warning in current realization, if operation succeeded 
	 * the directories structure will be rebuilded. This mean that 
	 * (!) all previous pointers to DDirectories except Root Directory
	 * will become deleted => unusable
	 * 
	 * @param newDirName Name of the directory. Contains A-Z, a-z,0-9, _, -, 
	 * @param parentDirFullPath Path of the parent directory. 
	 * @param comment comment on this dir. Might be NULL
	 * @return 
	 */
	virtual bool MakeDirectory(const string& newDirName, const string& parentDirFullPath, const string& comment = "");
	
	/** @brief Updates directory
	 *
	 * @warning in current realization, if operation succeeded 
	 * the directories structure will be rebuilded. This mean that 
	 * (!) all previous pointers to DDirectory objects except Root Directory
	 * will become deleted => unusable
	 * 
	 * @param  dir		Directory to update
	 * @return   bool
	 */
	virtual bool UpdateDirectory(Directory *dir);

	/**
	 * @brief Deletes directory using parent path
	 *
	 *	"/" - root directory can't be deleted
	 *
	 * @warning in current realization, if operation succeeded 
	 * the directories structure will be rebuilded. This mean that 
	 * (!) all previous pointers to DDirectories except Root Directory
	 * will become deleted => unusable
	 * 
	 * @param  [in] path Path of the directory. 
	 * @return true if no errors 
	 */
	virtual bool DeleteDirectory(const string& fullPath);

	/**
	 * @brief Deletes directory using directory object
	 *
	 *	"/" cant be deleted
	 *
	 * @warning in current realization, if operation succeeded 
	 * the directories structure will be rebuilded. This mean that 
	 * (!) all previous pointers to DDirectories except Root Directory
	 * will become deleted => unusable
	 *
	 * @param  [in] dir	Directory to Delete
	 * @return true if no errors 
	 */
	virtual bool DeleteDirectory(Directory *dir);
		
	/** @brief Load directories
	 *
	 * Explicitly forces to load directories from DB and build directory structure
	 * (!) At this implementation all existing directories references will be deleted, 
	 * thus  references to them will become broken
	 * @return   bool
	 */
	bool LoadDirectories(); ///Reads all directories from DB

	/** @brief Indicates ether we check each "GetDirectory" and other functions 
	 * that directories could be updated or not
	 *
	 * When we work in "edit" mode one needs to be sure the directories isnt updated while he works
	 * So every GetDirectory this soft check DB for updates since the last time
	 * But in the JANA "read only" mode it assumes that nobody will moe the directories
	 * thus we can reduce a lot of DB queries by not checking updates each time
	 *
	 * @return   bool
	 */
	bool GetCheckDirectoriesUpdate() const { return mNeedCheckDirectoriesUpdate; }

	/** @brief Sets ether we check each "GetDirectory" and other functions 
	 * that directories could be updated or not
	 *
	 * @see GetCheckDirectoriesUpdate
	 *
	 * @param     bool val
	 * @return   void
	 */
	void SetCheckDirectoriesUpdate(bool val) { mNeedCheckDirectoriesUpdate = val; }
	
	//----------------------------------------------------------------------------------------
	//	C O N S T A N T   T Y P E   T A B L E
	//----------------------------------------------------------------------------------------

	/** @brief Gets ConstantsType information from the DB
	 *
	 * @param  [in] path absolute path of the type table
	 * @return new object of DConstantsTypeTable
	 */
	virtual ConstantsTypeTable * GetConstantsTypeTable(const string& path, bool loadColumns=false);

	/** @brief Gets ConstantsType information from the DB
	 *
	 * @param  [in] name nfme of ConstantsTypeTable
	 * @param  [in] parentDir directory that contains type table
	 * @return new object of DConstantsTypeTable
	 */
	virtual ConstantsTypeTable * GetConstantsTypeTable(const string& name, Directory *parentDir, bool loadColumns=false);

	/** @brief Gets all "constants" from current directory
	 *
	 * @param parentDirPath
	 * @return success status
	 */
	virtual bool GetConstantsTypeTables(vector<ConstantsTypeTable *>& typeTables, const string& parentDirPath, bool loadColumns=false);

	/** @brief Gets all "constants" from current directory
	 *
	 * @param parentDirPath
	 * @return vector<DConstantsTypeTable *>
	 */
	virtual vector<ConstantsTypeTable *> GetConstantsTypeTables(const string& parentDirPath, bool loadColumns=false);

	/** @brief Get all "constants" from current directory
	 *
	 * @param parentDir
	 * @return success status
	 */
	virtual bool GetConstantsTypeTables(vector<ConstantsTypeTable *>& typeTables, Directory *parentDir, bool loadColumns=false);

	/** @brief Get all "constants" from current directory
	 *
	 * @param parentDir
	 * @return vector of constants
	 */
	virtual vector<ConstantsTypeTable *> GetConstantsTypeTables (Directory *parentDir, bool loadColumns=false);
		
	/** @brief Searches for type tables that matches the patten
	 *
	 * Searches type table that matches the pattern 
	 * inside parent directory with path @see parentPath 
	 * or globally through all type tables if parentPath is empty
	 * The pattern might contain
	 * '*' - any character sequence 
	 * '?' - any single character
	 *
	 * paging could be done with @see startWith  @see take
	 * startRecord=0 and selectRecords=0 means select all records;
	 *
	 * objects that are contained in vector<DDirectory *>& resultDirectories will be
	 * 1) if this provider owned - deleted (@see ReleaseOwnership)
	 * 2) if not owned - just leaved on user control
	 * @param  pattern
	 * @param  parentPath
	 * @return vector<DConstantsTypeTable *>
	 */
	virtual vector<ConstantsTypeTable *> SearchConstantsTypeTables(const string& pattern, const string& parentPath = "", bool loadColumns=false, int take=0, int startWith=0 );

	/** @brief Searches for type tables that matches the patten
	 *
	 * Searches type table that matches the pattern 
	 * inside parent directory with path @see parentPath 
	 * or globally through all type tables if parentPath is empty
	 * The pattern might contain
	 * '*' - any character sequence 
	 * '?' - any single character
	 *
	 * paging could be done with @see startWith  @see take
	 * startRecord=0 and selectRecords=0 means select all records;
	 *
	 * objects that are contained in vector<DDirectory *>& resultDirectories will be
	 * 1) if this provider owned - deleted (@see ReleaseOwnership)
	 * 2) if not owned - just leaved on user control
	 * @param  pattern
	 * @param  parentPath
	 * @return bool
	 */
	virtual bool SearchConstantsTypeTables(vector<ConstantsTypeTable *>& typeTables, const string& pattern, const string& parentPath = "", bool loadColumns=false, int take=0, int startWith=0 );
	
	/**
	 * @brief This function counts number of type tables for a given directory 
	 * @param [in] directory to look tables in
	 * @return number of tables to return
	 */
	virtual int CountConstantsTypeTables(Directory *dir);
	
	/** @brief Creates constant table in database
	 *
	 * @param   table
	 * @return bool
	 */
	virtual bool CreateConstantsTypeTable(ConstantsTypeTable *table);

	/** @brief Creates constant table in database
	 * 	
	 * Creates  constants type table in database and returns reference to
	 * created table if operation is succeeded (NULL otherwise)
	 * 
	 * The map "columns" should contains <"name", "type"> string pairs where:
	 *  -- "name" is the name of the column.
	 *      Should have the same naming convetions
	 *      as names of directories and type tables  @see ValidateName() 
	 *  -- "type" is the type of the column 
	 *     might be: int, uint, long, ulong, double, bool, string 
	 *     other values will lead to "double" type to be used.
	 *     @see ccdb::ConstantsTypeColumn::StringToType
	 *     Thus <"px", "">, <"py", ""> will create two double typed columns
	 * 
	 * @param [in] name			name of the new constants type table 
	 * @param [in] parentPath   parent directory path
	 * @param [in] rowsNumber  Number of rows
	 * @param [in] columns     a map fo "name", "type" pairs
	 * @param [in] comments		description for this type table
	 * @return NULL if failed, pointer to created object otherwise
	 */
	virtual ConstantsTypeTable* CreateConstantsTypeTable(const string& name, const string& parentPath, int rowsNumber, map<string, string> columns, const string& comments ="");
	
	/** @brief Creates constant table in database
	 * 	
	 * Creates  constants type table in database and returns reference to
	 * created table if operation is succeeded (NULL otherwise)
	 * 
	 * The map "columns" should contains <"name", "type"> string pairs where:
	 *  -- "name" is the name of the column.
	 *      Should have the same naming convetions
	 *      as names of directories and type tables  @see ValidateName() 
	 *  -- "type" is the type of the column 
	 *     might be: int, uint, long, ulong, double, bool, string 
	 *     other values will lead to "double" type to be used.
	 *     @see ccdb::ConstantsTypeColumn::StringToType
	 *     Thus <"px", "">, <"py", ""> will create two double typed columns
	 * 
	 * @param [in] name	       name of the new constants type table 
	 * @param [in] parentDir   parent directory
	 * @param [in] rowsNumber  Number of rows
	 * @param [in] columns     a map for "name"-"type" pairs
	 * @param [in] comments    description for this type table
	 * @return NULL if failed, pointer to created object otherwise
	 */
	virtual ConstantsTypeTable* CreateConstantsTypeTable(const string& name, Directory *parentDir, int rowsNumber, map<string, string> columns, const string& comments ="");

	/** @brief Uptades constant table in database
	 *
	 * Thes function updates only:
	 * - name
	 * - parent directory
	 * - commens
	 * The function doesnt change any columns and rows number
	 * It assumed, that a user should delete the TypeTable and recreate it 
	 * to perform such changes because it will break all data already stored
	 * @param   [in] table with updated info
	 * @return bool
	 */
	virtual bool UpdateConstantsTypeTable(ConstantsTypeTable *table);


	/** @brief Deletes constant type table
	 * 
	 * Deletes constant type table. 
	 * The type table will not be deleted if any assignment for this table exists
	 * Used should delete assignments first and only than delete the type table.
	 * 
	 * @param   table table info
	 * @return bool true if success
	 */
	virtual bool DeleteConstantsTypeTable(ConstantsTypeTable *table);

	/** @brief Loads columns for "table" type table
	 *
	 * @param [in out] table
	 * @return true if no errors (even if no columns loaded)
	 */
	virtual bool LoadColumns(ConstantsTypeTable* table);

	/** @brief Creates columns for the table
	 *
	 * @param parentDir
	 * @return vector of constants
	 */
	virtual bool CreateColumn(ConstantsTypeColumn* column);

	/** @brief Loads columns for the table
	 *
	 * @param parentDir
	 * @return vector of constants
	 */
	virtual bool CreateColumns(ConstantsTypeTable* table);
	

	//----------------------------------------------------------------------------------------
	//	R U N   R A N G E S
	//----------------------------------------------------------------------------------------

	/** @brief Creates RunRange in db
	 *
	 * TODO remove method
	 * @param     DRunRange * run
	 * @return   bool
	 */
	virtual bool CreateRunRange(RunRange *run);

	/** @brief GetRun Range from db
	 *
	 * @param     int min
	 * @param     int max
	 * @param     const char * name
	 * @return   DRunRange* return NULL if not found or failed
	 */
	virtual RunRange* GetRunRange(int min, int max, const string& name = "");

	/** @brief GetRun Range from db
	 *
	 * @param     int min
	 * @param     int max
	 * @param     const char * name
	 * @return   DRunRange* return NULL if not found or failed
	 */
	virtual RunRange* GetRunRange(const string& name);

	/** @brief Gets run range from DB Or Creates RunRange in DB
	 *
	 * @param min run ranges limits
	 * @param max run ranges limits
	 * @param name name of the run range
	 * @return   DRunRange* return NULL if failed
	 */
	virtual RunRange* GetOrCreateRunRange(int min, int max, const string& name="", const string& comment="");
	
	/**
	 * @brief Searches all run ranges associated with this type table
	 * 
	 * @param [out] resultRunRanges result run ranges
	 * @param [in]  table		table to search run ranges in
	 * @param [in]  variation	variation to search, if not set all variations will be selected
	 * @param [in]  take			how many records to take
	 * @param [in]  startWith	start record to take
	 * @return 
	 */
	virtual bool GetRunRanges(vector<RunRange *>& resultRunRanges, ConstantsTypeTable *table, const string& variation="", int take=0, int startWith=0 );
	
	/** @brief Updates run range
	 * 
	 * Function updates:
	 * - start run number
	 * - end run number
	 * - comment
	 * does not update run range name and id
	 * @param [in] run runrange model with proper id and updated data
	 * @return bool if no errors and data is changed
	 */
	virtual bool UpdateRunRange(RunRange* run);
	
	/** @brief Deletes run range
	 * @param [in] run model object to delete
	 * @return true if no errors and run range deleted
	 */
	virtual bool DeleteRunRange(RunRange* run);
	
	//----------------------------------------------------------------------------------------
	//	V A R I A T I O N
	//----------------------------------------------------------------------------------------
	/** @brief Load variation by name
	 * 
	 * @param     const char * name
	 * @return   DVariation*
	 */
	virtual Variation* GetVariation(const string& name);
	
	/**
	 * @brief Searches all variations associated with this type table
	 * @param  [out] resultVariations result variations
	 * @param  [in]  table table to search run ranges in
	 * @param  [in]  run   specified run to search for, if 0 searches for all run ranges
	 * @param  [in]  take how many records to take
	 * @param  [in]  startWith start record to take
	 * @return 
	 */
	virtual bool GetVariations(vector<Variation *>& resultVariations, ConstantsTypeTable *table, int run=0, int take=0, int startWith=0 );
	
	/**
	 * @brief Searches all variations associated with this type table
	 
	 * @param  [in]  table table to search run ranges in
	 * @param  [in]  run   specified run to search for, if 0 searches for all run ranges
	 * @param  [in]  take how many records to take
	 * @param  [in]  startWith start record to take
	 * @return resultVariations result variations
	 */
	virtual vector<Variation *> GetVariations(ConstantsTypeTable *table, int run=0, int take=0, int startWith=0 );


	/** @brief Create variation
	 *
	 * @param     DVariation * variation
	 * @return   bool
	 */
	virtual bool CreateVariation(Variation *variation);

	/** @brief Update variation
	 *
	 * Function updates:
	 * variation comments
	 * variation name if there is no variations with this name
	 * 
	 * @param   [in]  variation to update
	 * @return true if success  
	 */
	virtual bool UpdateVariation(Variation *variation);

	/** @brief Delete variation
	 *
	 * Variation will be deleted only 
	 * if there is no assignments for this variation
	 * @param    [in] variation to delete
	 * @return   bool
	 */
	virtual bool DeleteVariation(Variation *variation);
	
	//----------------------------------------------------------------------------------------
	//	A S S I G N M E N T S
	//----------------------------------------------------------------------------------------
	/** @brief Get Assignment with data blob only
     *
     * This function is optimized for fast data retrieving and is assumed to be performance critical;
     * This function doesn't return any specified information like variation object or run-range object
     * @see GetAssignmentFull
     * @param [in] run - run number
     * @param [in] path - object path
     * @param [in] variation - variation name
     * @return DAssignment object or NULL if no assignment is found or error
     */
	virtual Assignment* GetAssignmentShort(int run, const string& path, const string& variation="default");
	
	
	 /** @brief Get specified by creation time version of Assignment with data blob only.
     *
     * The Time is a timestamp, data that is equal or earlier in time than that timestamp is returned
     *
     * @remarks this function is named so
     * @param [in] run - run number
     * @param [in] path - object path
     * @param [in] time - timestamp, data that is equal or earlier in time than that timestamp is returned
     * @param [in] variation - variation name
     * @return new DAssignment object or 
     */
    virtual Assignment* GetAssignmentShort(int run, const string& path, time_t time, const string& variation="default");
    
     /** @brief Get specified version of Assignment with data blob only
     *
     * @remarks this function is named so
     * @param [in] run - run number
     * @param [in] path - object path
     * @param [in] version - version is number of constants beginning with first added constant set. So 1 is the first added constants
     * @param [in] variation - variation name
     * @return new DAssignment object or 
     */
    virtual Assignment* GetAssignmentShortByVersion(int run, const string& path, int version, const string& variation="default");
    
	/** @brief Get last Assignment with all related objects
	 *
	 * @param     int run
	 * @param     path to constant path
	 * @return NULL if no assignment is found or error
	 */
	virtual Assignment* GetAssignmentFull(int run, const string& path, const string& variation="default");
	
	
	/** @brief  Get specified version of Assignment with all related objects
	 *
	 * @param     int run
	 * @param     const char * path
	 * @param     const char * variation
	 * @param     int version
	 * @return   DAssignment*
	 */
	virtual Assignment* GetAssignmentFull(int run, const string& path, int version, const string& variation="default");
		
	/** @brief Creates Assignment using related object
	 * 
	 * @warning since composing a "NEW" DAssignment is complex operation 
	 *          it is suggested to use other reloads of this function for users 
	 *          for creating "new" assignments. 
	 *          This function is not set as "protected" only to provide easy "copy" operations
	 *	 
	 * This function assumes that all needed data is already contained in assignment
	 * object. Including correct type table and data blob. 
	 *
	 * The function is powerful to copy assignment. For example from one variation to another.
	 *
	 * The function changes DAssignment object by setting Id and DataVaultId by new values
	 *
	 * @param    DAssignment * assignment
	 * @return   DAssignment* adjusted with Id-s of objects and etc
	 */
	virtual bool CreateAssignment(Assignment *assignment);

	/** @brief Creates Assignment using related object
	 *
	 * Creates Assignment using related object.
	 * 
	 * Validation:
	 * If no such run range found, the new will be created (with no name)
	 * No action will be done (and NULL will be returned):
	 * -- If no type table with such path exists
	 * -- If data is inconsistent with columns number and rows number
	 * -- If no variation with such name found
	 * 
	 * @brief 
	 * @param data  		by rows and columns
	 * @param runMin		run range minimum
	 * @param runMax     run range maximum
	 * @param variationName	name of variation
	 * @param comments   comments
	 * @return NULL if failed, DAssignment reference if success
	 */
	virtual Assignment* CreateAssignment(const std::vector<std::vector<std::string> >& data, const std::string& path, int runMin, int runMax, const std::string& variationName, const std::string& comments);

	/**
	 * @brief Complex and universal function to retrieve assignments
	 * 
	 * @warning this function is too complex for users. 
	 *          Use overloaded GetAssignments instead of it.
	 *          And only use this function if others are inappropriate
	 * 
	 * This function is universal assignments getter. 
	 * Provided fields allows to select assignments (arrays and single assignments) 
	 * for most cases. Other GetAssignments(...) and GetAssignmentFull(...) 
	 * functions relie on this function.  
	 *
	 * 
	 * runMin, runMax:
	 *       applied if one !=0. Thus runMin=runMax=0 will select all run ranges
	 *       If one needs particular run, runMin=runMax=<NEEDED RUN NUMBER> should be used
	 * 
	 * runRangeName:
	 *       will be ignored if equals to ""
	 * 
	 * variation:
	 *       if "", all variations will be 
	 * 
	 * date: 
	 *       unix timestamp that indicates the latest time to select assignments from
	 *       if 0 - time will be ignored
	 * sortby:
	 *       0 - `assignments`.`created` DESC
	 *       1 - `assignments`.`created` ASC
	 * 
	 * take, startWith
	 *       paging parameters
	 * 
	 * @param [out] assingments result assignment list
	 * @param [in] path		  path of type table
	 * @param [in] run		  specified range. If not set all ranges
	 * @param [in] variation  variation, if not set, all variations
	 * @param [in] date		  nearest date
	 * @param [in] sortBy	  sortby order
	 * @param [in] startWith  record to start with
	 * @param [in] take		  records to select
	 * @return true if no errors (even if no assignments was selected)
	 */
	virtual bool GetAssignments(vector<Assignment *> &assingments,const string& path, int runMin, int runMax, const string& runRangeName, const string& variation, time_t beginTime, time_t endTime, int sortBy=0, int take=0, int startWith=0);
	

	/**
	 * @brief Get assigments for particular run
	 * 
	 * returns vector of assignments
	 * Variation: if variation is not empty string the assignments for specified variation will be returned
	 *            otherwise all variations will be accepted
	 * 
	 * Date: if date is not 0, assignments which are earlier than this date will be returned
	 *       otherwise returned assignments will be not filtered by date
	 * 
	 * Paging: paging could be done with  @see take ans @see startWith 
	 *         take=0, startWith=0 means select all records;
	 * 
	 * 
	 * @param [out] assingments
	 * @param [in]  path
	 * @param [in]  run
	 * @param [in]  variation
	 * @param [in]  date
	 * @param [in]  take
	 * @param [in]  startWith
	 * @return 
	 */
	virtual bool GetAssignments(vector<Assignment *> &assingments,const string& path, int run, const string& variation="", time_t date=0, int take=0, int startWith=0);

	/**
	 * @brief Get assigments for particular run
	 * 
	 * returns vector of assignments
	 * Variation: if variation is not empty string the assignments for specified variation will be returned
	 *            otherwise all variations will be accepted
	 * 
	 * Date: if date is not 0, assignments which are earlier than this date will be returned
	 *       otherwise returned assignments will be not filtered by date
	 * 
	 * Paging: paging could be done with  @see take ans @see startWith 
	 *         take=0, startWith=0 means select all records;
	 * 
	 * 
	 * @param [out] assingments
	 * @param [in]  path
	 * @param [in]  run
	 * @param [in]  variation
	 * @param [in]  date
	 * @param [in]  take
	 * @param [in]  startWith
	 * @return 
	 */
	virtual vector<Assignment *> GetAssignments(const string& path, int run, const string& variation="", time_t date=0, int take=0, int startWith=0);

	/**
	 * @brief Get assigments for particular run
	 * 
	 * returns vector of assignments
	 * Variation: if variation is not empty string the assignments for specified variation will be returned
	 *            otherwise all variations will be accepted
	 * 
	 * Date: if date is not 0, assignments which are earlier than this date will be returned
	 *       otherwise returned assignments will be not filtered by date
	 * 
	 * Paging: paging could be done with  @see take ans @see startWith 
	 *         take=0, startWith=0 means select all records;
	 * 
	 * 
	 * @param [out] assingments
	 * @param [in]  path
	 * @param [in]  run
	 * @param [in]  variation
	 * @param [in]  date
	 * @param [in]  take
	 * @param [in]  startWith
	 * @return 
	 */
	virtual bool GetAssignments(vector<Assignment *> &assingments,const string& path, const string& runName, const string& variation="", time_t date=0, int take=0, int startWith=0);

	/**
	 * @brief Get assigments for particular run
	 * 
	 * returns vector of assignments
	 * Variation: if variation is not empty string the assignments for specified variation will be returned
	 *            otherwise all variations will be accepted
	 * 
	 * Date: if date is not 0, assignments which are earlier than this date will be returned
	 *       otherwise returned assignments will be not filtered by date
	 * 
	 * Paging: paging could be done with  @see take ans @see startWith 
	 *         take=0, startWith=0 means select all records;
	 * 
	 * 
	 * @param [out] assingments
	 * @param [in]  path
	 * @param [in]  run
	 * @param [in]  variation
	 * @param [in]  date
	 * @param [in]  take
	 * @param [in]  startWith
	 * @return 
	 */
	virtual vector<Assignment *> GetAssignments(const string& path, const string& runName, const string& variation="", time_t date=0, int take=0, int startWith=0);

	/**
	 * @brief the function updates assignment comments
	 * @param assignment
	 * @return 
	 */
	virtual bool UpdateAssignment(Assignment* assignment);
	
	/**
	 * @brief Deletes assignment
	 * @param assignment object to delete. Must have valid ID 
	 * @return true if success
	 */
	virtual bool DeleteAssignment(Assignment* assignment);
	
	/**
	 * @brief Fill assignment with data if it has proper ID
	 * 
	 * The function actually gets assignment by ID. 
	 * 
	 * A problem: is that for DB providers the id is assignment->GetId().
	 *            For file provider the id is probably a type table full path
	 * A solution: 
	 *    is to have this function that accepts DAssignment* assignment.
	 *    DAssignment* assignment incapsulates the id (one way or another)
	 *    And each provider could check if this DAssignment have valid Id,
	 *    fill assignment with data and return true. 
	 *    Or just return "false" if something goes wrong;
	 * 
	 * @param [in, out] assignment to fill
	 * @return true if success and assignment was filled with data
	 */
	virtual bool FillAssignment(Assignment* assignment);
	
protected:
	
	/** @brief Adds Log Record to db
	 *
	 * @param     userName
	 * @param     affectedTables
	 * @param     affectedIds
	 * @param     shortDescription
	 * @param     fullDescription
	 * @return   void
	 */
	void AdLogRecord(string userName, string affectedTables, string affectedIds, string shortDescription, string fullDescription);
	
	/** @brief Adds Log Record, uses GetLogUserName() as user name
	 *
	 * @param     affectedTables
	 * @param     affectedIds
	 * @param     shortDescription
	 * @param     fullDescription
	 * @return   void
	 */
	void AdLogRecord(string affectedTables, string affectedIds, string shortDescription, string fullDescription);
	
	/** @brief Gets user Id by name from appropriate table
	 *
	 * @param     userName
	 * @return   dbkey_t
	 */
	dbkey_t GetUserId(string userName);

	virtual bool QuerySelect(const char* query);	///Do "select" queries 
	virtual bool QuerySelect(const string& query);	///Do "select" queries 
	virtual bool QueryInsert(const char* query);	///Do "insert" queries 
	virtual bool QueryInsert(const string& query);	///Do "insert" queries
	virtual bool QueryUpdate(const char* query);	///Do "Update" queries 
	virtual bool QueryUpdate(const string& query);	///Do "Update" queries;
	virtual bool QueryDelete(const char* query);	///Do "Delete" queries 
	virtual bool QueryDelete(const string& query);	///Do "Delete" queries 
	virtual bool QueryCustom(const string& query);	///Do any custom queries queries 
	
	virtual void FreeMySQLResult();	///Frees my sql result manually
	
	/** @brief gets last error and description and composes it as a string
	 * @return string with composed error
	 */
	std::string ComposeMySQLError(std::string mySqlFunctionName="");
	
	/** @brief Fetches next Row
	 *
	 * @return true if row was read, false if no more rows
	 */
	bool FetchRow();

    /** prepares preparedStatemets for use
     */
    bool InitializePreparedStatements(){;}; //TODO implement PreparedStatements for GetAssignmentShort funcitons

	//read of row fields
	bool IsNullOrUnreadable(int fieldNum);		///Check if the field is NULL or is unreadable. If it is Unreadable
	int				ReadInt(int fieldNum);		///Reads int	from the last query row
	dbkey_t			ReadIndex(int fieldNum);	///Reads dbkey_t from the last query row
	unsigned int	ReadUInt(int fieldNum);		///Reads unsigned int from the last query row
	long			ReadLong(int fieldNum);		///Reads long from the last query row
	unsigned long	ReadULong(int fieldNum);	///Reads unsigned long from the last query row
	bool			ReadBool(int fieldNum);		///Reads bool from the last query row
	double			ReadDouble(int fieldNum);	///Reads double from the last query row
	string			ReadString(int fieldNum);	///Reads string from the last query row
	time_t			ReadUnixTime(int fieldNum); ///Reads string from the last query row
	void BuildDirectoryDependences();			///Builds directory relational structure. Used right at the end of RetriveDirectories().
	bool CheckDirectoryListActual();			///Checks if directory list is actual i.e. nobody changed directories in database
	bool UpdateDirectoriesIfNeeded();
	
	/** @brief
	 * Returns string "NULL" if comment is NULL otherwise 
	 * return encoded comment for INSERT/UPDATE statement
	 *
	 * @param	[in] comment
	 * @return string to incert in query
	 */
	string PrepareCommentForInsert(const string& comment);
	
	/**
	 * @brief Converts wildcard * and ? to MySQL like % _ in the string
	 * @param str string to convert
	 * @return converted string
	 */
	virtual string WilcardsToLike(const string& str); ///Prepares search pattern for MySQL
		
	virtual string PrepareLimitInsertion(int take=0, int startWith=0);
//---------------------------------------------------------------
//  DEBBUGING AND OPTIMIZATION
//----------------------------------------------------------------
	string LastFullQuerry() const { return mLastFullQuerry; }
	string LastShortQuerry() const { return mLastShortQuerry; }

private:
	
	
	MYSQL *mMySQLHnd;			//Handler to mysql object
	bool mIsStoredObjectOwner;
	Assignment* FetchAssignment(ConstantsTypeTable *table);
	virtual void FetchAssignment(Assignment* assignment, ConstantsTypeTable *table);

  

	/******* D I R E C T O R I E S   W O R K *******/ 
	vector<Directory *>  mDirectories;
	map<dbkey_t,Directory *> mDirectoriesById;
	map<string,Directory *> mDirectoriesByFullPath;
	bool mDirsAreLoaded; 
	bool mNeedCheckDirectoriesUpdate; //Do we need to check each time iff directories are updated or not
	
	bool mHaveUnfreeResults; 			//indicates that we have some unfree results from mysql, that must be freed 
	MYSQL_RES *mResult;					//result from last queue
	MYSQL_ROW mRow;						//last fetched row
	MYSQL_ULONG mReturnedRowsNum;		//number of returned rows from last SELECT query
	MYSQL_ULONG mReturnedFieldsNum;		//number of returned fields from last SELECT query
	MYSQL_ULONG mReturnedAffectedRows;	//number of affected rows from last INSERT, DELETE, UPDATE queries
	MYSQL_ULONG mLastInsertedId;		//number of last id
	Directory *mRootDir;				//root dir
	bool mIsConnected;					//indicates connection to db
	static bool mMySqlIsInitialized;	//flag that mysql is initialized

	string mLastFullQuerry;   //full text of last full get assignment query
	
	string mLastShortQuerry;  //full text of last short assignment query
	
    //VARIATIONs WORK
    dbkey_t GetVariationId( const string& name ); ///Gets mysql record Id for specified variation name
    dbkey_t mLastVariationId;                     ///Last requested variation ID. Used for caching
    string mLastVariationName;                    ///Last requested variation name. Used for caching
	

};
}

#endif //_DMySQLDataProvider_
