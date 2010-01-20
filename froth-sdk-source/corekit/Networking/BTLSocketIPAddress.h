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

#import <Foundation/Foundation.h>
#import "BTLSocketAddress.h"
#import <sys/types.h>
#import <netinet/in.h>

@interface BTLSocketIPAddress : BTLSocketAddress {
}

#ifdef KEEP_UNDEFINED
#pragma mark Initializers
#endif

+ (id)addressWithHostname:(NSString*)aHostname port:(in_port_t)aPort error:(NSError**)anError;
+ (id)addressWithHostname:(NSString*)aHostname port:(in_port_t)aPort family:(sa_family_t)aFamily
					error:(NSError**)anError;

- (id)initWithHostname:(NSString*)aHostname port:(in_port_t)aPort error:(NSError**)anError;

#ifdef KEEP_UNDEFINED
#pragma mark Accessors
#endif

- (BOOL)setHostname:(NSString*)aHostname error:(NSError**)anError;
- (NSString*)address;
- (NSString*)hostname;

- (BOOL)setPort:(in_port_t)aPort error:(NSError**)anError;
- (in_port_t)port;
- (NSString*)serviceForProtocol:(int)aProtocol;

@end
