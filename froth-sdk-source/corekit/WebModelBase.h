//
//  WebModelBase.h
//  Froth
//
//  Created by Allan Phillips on 14/07/09.
//
//  Copyright (c) 2009 Thinking Code Software Inc. http://www.thinkingcode.ca
//
//	Permission is hereby granted, free of charge, to any person
//  obtaining a copy of this software and associated documentation
//	files (the "Software"), to deal in the Software without
//	restriction, including without limitation the rights to use,
//	copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the
//	Software is furnished to do so, subject to the following
//	conditions:

//	The above copyright notice and this permission notice shall be
//	included in all copies or substantial portions of the Software.

//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//	OTHER DEALINGS IN THE SOFTWARE.

#import <Foundation/Foundation.h>

@class WebRequest;

/*!
	\brief	Provides the base model for dynamic model creation.
	\detail	By defualt models use Amazon SimpleDB, however subclasses could implement
				their own model connection (for example, see the MySqlModelBase class (opensource)
 
				Objects that implement WebModelBase are typically usable in any template view by using language 
				similar to Cocoa key value coding access. For example
				\code
				<h1>{{ data.keyName }}</h1>
				\endcode
 
				This abstract base provides a very limited implementation. And should be subclassed for greator control
 
				Provides built in support for.
				\code
				- (id)findFirstBy_<propertyName>:(NSString*)name
				- (id)findAllBy_<propertyName>:(NSString*)name
				\endcode
	
				NSCopying support allows for creation of new objects using existing implementation, however the uuid is
				not transfered over and the new object is not saved to the datasource yet.
  
				Subclasses dont need to specifically implement methods. They simply provide the class stucture
				and also any dataSource specific requirements (See SDBDataSource.h).
 
				To avoid compiler warnings (except in model @implementation), add getter/setter method declerations in model's header.
				\code
				- (void)set<Key>:(id)value;
				- (id)<Key>
				\endcode
 
				All keys must be defined in +allPersistableKeys otherwise an exception is thrown and in the implementation @dynamic key to suppress warnings.
 
				<h3>Property Validation Support</h3>
				
				<br><i>Example</i>
				\code
				- (BOOL)validateTitle:(id*)title msg:(NSString**)msg {
					if([*title hasPrefix:@"bad"]) {
						*msg = @"Titles must not have prefixes 'bad'";
						return froth_false;
						
						//or optionally modify the title and return true
						/
							*title = @"new value";
							return froth_true;
						/
					}
					return true;
				}
				\endcode
 
				Returns false if validation should fail, or true if passed. Validatators can also optionally update the value
				dynamically providing a new value based on validation rules.
*/
@interface WebModelBase : NSObject <NSCopying> /* Also implements JSONRepresentation */ {
	BOOL		m_notPersisted;
	NSString*	uid;
	
	@private
	NSMutableDictionary* m_data;
	NSMutableArray* m_dirtyKeys;
	NSMutableDictionary* m_datasource_data;
}
//Class and actual name as set by -identifierName and -identifierClass
@property (nonatomic, retain) NSString* uid;

/*
	WebDataSources should call this to initial an object based on existing datasource data
 */
- (id)initFromDatabase;

/*
	Subclasses should simply define their properties as normal object -c 2.0 properties, or kvo accessable properties
	or by implementing valueForKey and setValue:forKey kvo methods
 */

//The data source name to use. The defualt is "Defualt"
+ (NSString*)dataSourceName;

//Subclasses can overide this to provide a custom model name, the defualt is the Class name -Model <RemoteModelName> or <Class>Model
+ (NSString*)modelName;

//For alternate identifer names, If this does not match the datasource's internal name, then implementations should use +dataSourceKeyForPersistableKey for replacing
+ (NSString*)identifierName;

//For integers return NSNumber, for guids return NSString. Defualt is NSString/guid
+ (Class)identifierClass;

//Must return the list of all persistable keys
+ (NSArray*)allPersistableKeys;

//Subclasses can overide this to return TRUE for dynamicly adding new values with -setValue:forKey: without haveing a schema. Some models may not support this. */
+ (BOOL)hasStaticKeys;

/*
	Offers a subclass the ability to substatute a datasource name for a local property name, Data sources
	can take this into account in their implementations.
 
	Typically this is used in correspondance with 
	+ (NSString*)persistableKeyForDataSourceKey:(NSString*)key
	for the reverse direction.
 */
+ (NSString*)dataSourceKeyForPersistableKey:(NSString*)key;
+ (NSString*)persistableKeyForDataSourceKey:(NSString*)key;

//Standard Lookups / Creation
+ (NSArray*)all;
+ (NSArray*)findAllWithConditions:(NSDictionary *)conditions;

+ (id)findFirstWithConditions:(NSDictionary *)conditions;
+ (int)countWithConditions:(NSDictionary *)conditions;

/*! 
	\brief	This may not be supported by all datasources, and each source will have its own specification for the query syntax, returns nil if not supported.
	\detail	This should not be used in all cercumstances. Only in very specific cases does it make sence to use this, as it does
				not provides abstraction from the datasource, and calls to this may not be cross datasource compatible.
 
				This only makes sence when very specialized queries are needed for a specific datasoruce.
 */
+ (NSArray*)findWithQuery:(NSString*)query;

/*! \brief get a object with given identifier*/
+ (id)objectWithIdentifier:(NSString *)identifier;

/*! \brief Defualt initializer for newly created objects */
- (id)init;

/*! \brief Creates a object, populated with json dictionary data, not retained or autoreleased */
+ (id)createWithProperties:(NSDictionary*)dictionary;

/*! \brief Creates a object, populated with xml property data */
+ (id)createWithXML:(NSXMLNode*)node;

/*! \brief Convienence wrapper for creating via posts, accepts json or xml post content types. (uses -initWithProperties or -initWithXML) */
+ (id)createWithPostRequest:(WebRequest*)request;

//Also supports...
/*
  -find<All | First>By_<key>:(id)keyValue;
  -find<All | First>By_<key>:(id)keyValue <or | and>_<key>:(id)keyValue;
  -find<All | First>By_<key>:(id)keyValue and_<key>:(id)keyValue and_<key>:(id)keyValue;
  -countOf_<key>:(id)keyValue;
 */

//Calls the dataSource's implementation for transactional support
+ (void)beginTransactions;
+ (void)endTransactions;

/*!
	\brief	Performs a object's save operation directly, unless a call to +beginTransaction was made.
 */
- (BOOL)save;

- (void)didSaveForCreate;
- (void)didSaveForDelete;
- (void)didSaveForUpdate;

/*!
	\brief Deletes an object immediatly from the datasource
	@return	If the delete was successful
	
	The object should not do a subsequent save. Doing so will cause an exception to be thrown.
 */
- (BOOL)delete;

//Basic Structure Info
- (BOOL)hasAttribute:(NSString*)key;

//An array of keys that have changes since last save
- (NSArray*)dirtyKeys;
- (BOOL)isDirty;
- (void)dirty:(NSString*)key;
- (void)makeClean;

/*
	This data can be used by datasources for storing data source specific info. Subclasses
	can utilize this data according to the datasources specifictionm
 */
- (NSMutableDictionary*)dataSourceData;

/*
	Returns the current backing data dictionary for the current state of the object
	Data sources that allow for write all, should use this if +hasStaticKeys returns NO
 */
- (NSDictionary*)data;

/*!
	\brief Subclasses can overide this and include extra key/values to be included in the seralization of the object
 
	This essentially allows for a one-way encoding to a json string. Currently their is
	no support for getting a WebModelBase object from a json string.
 
	Essenstailly only the "allPersistableKeys" provided are supported as a raw json string unless this is overridden my subclasses
 
	All objects must be considered seralizable according to Froth seralization rules (NSDictionary, NSNumber, NSDate, NSArray, <Seralizable> adhearing classes)
 */
- (NSDictionary*)dictionaryRepresentation;

@end

/*!
	\brief Provides a WebModelBase proxy object for doing isEqual:other object based on uids
	\detail
	This is useful for doing things like
	[realModelObject isEqual:[WebModelBaseProxy with:aguid]];
 */
@interface WebModelBaseProxy : NSObject {
	NSString* uid;
}
@property (nonatomic, retain) NSString* uid;

+ (id)with:(NSString*)aGUID;

@end
