// Copyright (c) 2007-2008 Michael Buckley

// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#import "BTLSocketHandler.h"
#import "BTLSocketHandlerList.h"

//! The BTLSocketHandler class implements a node in a linked list which filters
//! data as it passes between the socket and its delegate.
//!
//! Many of the methods are abstract and should be implemented by a subclass.
//! They are identified in the documentation. If an instance of this class or
//! one of its subclasses recieves a message which it does not understand, it
//! will forward the message to the previous BTLSocketHandler in the list.
//!
//! N.B When instances of BTLSocketHandler are added to a BTLSocketHandlerList,
//! a copy of the BTLSocketHandler is added in place of the instance. It is thus
//! important to override copyWithZone:() in any subclass of BTLSocketHandler
//! to properly copy the instance.

@implementation BTLSocketHandler

#ifdef KEEP_UNDEFINED
#pragma mark Copying
#endif

- (id)copyWithZone:(NSZone*)zone
{
	id ret = [[self class] new];
	return ret;
}

#ifdef KEEP_UNDEFINED
#pragma mark Connecting and Disconnecting
#endif

//! \brief This method is called when the socket opens a connection to a remote
//! address.
//!
//! When this method is called, the BTLSocketHandlerList has been activated
//! up to this point in the list. This method can call writeData:toAddress() to
//! send data to the remote socket. Any response generated by sending out data
//! in this manner will be passed in to readData:fromAddress().
//!
//! When the BTLSocketHandler is finished opening the connection it should call
//! finishedOpeningConnectionToAddress:() from any method.
//!
//! This method is abstract and should be implemented by a subclass.


- (void)connectionOpenedToAddress:(BTLSocketAddress*)anAddress
{
	[self finishedOpeningConnectionToAddress:anAddress];
}

//! \brief This method should be called when the BTLSocketHandler is finished
//! opening the connection.

- (void)finishedOpeningConnectionToAddress:(BTLSocketAddress*)anAddress
{
	if([self nextHandler] != nil){
		[[self nextHandler] connectionOpenedToAddress:anAddress];
	}else{
		id delegate = [[[self encapsulatingList] socket] delegate];
		if([delegate respondsToSelector:@selector(connectionOpenedToAddress:sender:)]){
			[delegate connectionOpenedToAddress:anAddress sender:[[self encapsulatingList] socket]];
		}
	}
}

//! This method is called when the connection to the remote socket is closed.
//!
//! When this method is called, the BTLSockethandler list has been closed beyond
//! this point in the list. The BTLSocketHandler can still send and recieve data
//! until connectionFailedToAddress:() is called.
//!
//! This method is abstract and should be implemented by a subclass.

- (void)closeConnectionToAddress:(BTLSocketAddress*)anAddress
{
	[self finishedClosingConnectionToAddress:anAddress];
}

//! \brief This method should be called when the BTLSocketHandler is finished
//! closing the connection.

- (void)finishedClosingConnectionToAddress:(BTLSocketAddress*)anAddress
{
	if([self prevHandler] != nil && [self prevHandler] != [[self encapsulatingList] socket]){
		[((BTLSocketHandler*) [self prevHandler]) closeConnectionToAddress:anAddress];
	}else if([self prevHandler] == [[self encapsulatingList] socket]){
		[[self prevHandler] finishedClosingConnectionToAddress:anAddress];
	}
}

//! \brief This method is called when the socket failed to establish a
//! connection.
//!
//! This method is called automatically on every BTLSocketHandler in the list.
//! When it is called, the handler can not read data from or write data to the
//! socket
//!
//! This method is abstract and should be implemented by a subclass.

- (void)connectionFailedToAddress:(BTLSocketAddress*)anAddress
{
	// This method intentionally left blank.
}

//! \brief This method is called when the connection to the remote address is
//! interrupted.
//!
//! This method is called automatically on every BTLSocketHandler in the list.
//! When it is called, the handler can not read data from or write data to the
//! socket
//!
//! This method is abstract and should be implemented by a subclass.

- (void)connectionInterruptedToAddress:(BTLSocketAddress*)anAddress
{
	// This method intentionally left blank.
}

//! \brief This method is called when the socket failed to establish a
//! connection.
//!
//! This method is called automatically on every BTLSocketHandler in the list in
//! reverse order. When it is called, the handler can not read data from the
//! socket, but can write data to it.
//!
//! This method is abstract and should be implemented by a subclass.

- (void)connectionClosedByRemoteAddress:(BTLSocketAddress*)anAddress
{
	// This method intentionally left blank.
}

#ifdef KEEP_UNDEFINED
#pragma mark Reading and Writing
#endif

//! \brief This method is called when the socket receives data from the remote
//! socket.
//!
//! If this method is going to modify the data, it should first make a copy of
//! the data using [someData copy], and modify that copy instead of the
//! original. When finished, this method should call
//! finishedReadingData:fromAddress:().
//!
//! This method is abstract and should be implemented by a subclass.

- (void)readData:(BTLSocketBuffer*)someData fromAddress:(BTLSocketAddress*)anAddress
{
	[self finishedReadingData:someData fromAddress:anAddress];
}

//! \brief This method is called when the BTLSocketHandler is finished reading
//! data from the socket.

- (void)finishedReadingData:(BTLSocketBuffer*)someData fromAddress:(BTLSocketAddress*)anAddress
{
	int64_t position = [someData position];
	[someData rewind];
	if([self nextHandler] != nil){
		[[self nextHandler] readData:someData fromAddress:anAddress];
	}else{
		id delegate = [[[self encapsulatingList] socket] delegate];
		if([delegate respondsToSelector:@selector(readData:fromAddress:sender:)]){
			[delegate readData:someData fromAddress:anAddress sender:[[self encapsulatingList] socket]];
		}
	}
	[someData setPosition:position];
}

//! \brief This method is called before sending the data to the socket.
//!
//! If this method is going to modify the data, it should first make a copy of
//! the data using [someData copy], and modify that copy instead of the
//! original. When finished, this method should call
//! finishedWritingData:fromAddress:().
//!
//! This method is abstract and should be implemented by a subclass.

- (BOOL)writeData:(BTLSocketBuffer*)someData toAddress:(BTLSocketAddress*)anAddress
{
	return [self finishedWritingData:someData toAddress:anAddress];
}

//! \brief This method is called when the BTLSocketHandler is finished writing
//! data to the socket.

- (BOOL)finishedWritingData:(BTLSocketBuffer*)someData toAddress:(BTLSocketAddress*)anAddress
{
	int64_t position = [someData position];
	[someData rewind];
	
	BOOL ret;
	
	if([self prevHandler] != nil && [self prevHandler] != [[self encapsulatingList] socket]){
		ret = [[self prevHandler] writeData:someData toAddress:anAddress];
	}else if([self prevHandler] == [[self encapsulatingList] socket]){
		ret = [[self prevHandler] finishedWritingData:someData toAddress:anAddress];
	}
	[someData setPosition:position];
	
	return ret;
}

#ifdef KEEP_UNDEFINED
#pragma mark Accessors
#endif

//! \brief This method removes the BTLSocketHandler from the list.

- (void)removeSelf
{
	[[self encapsulatingList] removeHandler:self];
	
	if([self nextHandler] != nil){
		[[self nextHandler] setPrevHandler:[self prevHandler]];
	}
	
	if([self prevHandler] != nil){
		if([[self prevHandler] respondsToSelector:@selector(setNextHandler:)]){
			[[self prevHandler] setNextHandler:[self nextHandler]];
		}
	}
	
	[self setNextHandler:nil];
	[self setPrevHandler:nil];
	[self release];
}

//! \brief This method removes this handler and all after it from the list.

- (void)removeAll
{
	if([self nextHandler] != nil){
		[[self nextHandler] removeAll];
	}
	
	[self removeSelf];
}

- (void)setNextHandler:(BTLSocketHandler*)aHandler
{
	if(aHandler == nextHandler){
		return;
	}

	nextHandler = aHandler;
}

- (void)setPrevHandler:(id)aHandler
{
	if(aHandler == prevHandler){
		return;
	}
	
	prevHandler = aHandler;
}

- (BTLSocketHandler*)nextHandler
{
	return nextHandler;
}

- (id)prevHandler
{
	return prevHandler;
}

- (void)setEncapsulatingList:(BTLSocketHandlerList*)aList
{
	// Avoid a retain loop.
	encapsulatingList = aList;
}

- (BTLSocketHandlerList*)encapsulatingList
{
	return encapsulatingList;
}

- (void)forwardInvocation:(NSInvocation *)anInvocation
{
	[anInvocation invokeWithTarget:[self prevHandler]];
}

@end
