//
//  WebActionController.h
//  Froth
//
//  Created by Crystal Phillips on 26/06/09.
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
@class WebResponse;
@class WebActionView;
@class WebLayoutView;
@protocol WebComponent;


/*!
	\brief	Action controllers are the heart of a Cocoa Web App, they are considered short lived
				objects and must implement proper memory management to clean up after themselves. They
				only live for the lifespan of the request.
 
	\detail	Class names have the format WA<Name>Controller
				where the request would be controller-name or controller_name or controllerName
			
				Actual classes should use the "WebActiveController, and other implementing classes
				for better access."
 
				Built in automatic actions are as follows.
				A) {{method-name}} automatically maps to - (id)<methodName>Action:(WebRequest*)wr;
				B) No Method (GET)				 maps to -(id)index:(WebRequest*)wr;
							 (GET)/{v}			 maps to -(id)object:(WebRequest*)wr;
				C) No Method (POST/POST)		 maps to -(id)create:(WebRequest*)wr; /-update:...
				E) No Method (DELETE)			 maps to -(id)delete:(WebRequest*)wr;
 
				The flow of events for a Web Request is as such.
				1. WebApplicationController receives a WebRequest.
				2. if controller provides "components" array, call, -preProcessRequest: forController: on each continuing if result is not nil.
				2. WebApplicationController calls - (void)preProcessRequest:(WebRequest*)request if implemented.
				3. WebApplicationController calls - (SEL)selectorForActionName:(NSString*)name if implemented, and uses it as <ActionName>
				4. WebApplicationController insures that <ActionName> is implemented.
				5. WebApplicationController calls - (void)init<ActionName>Action:(WebRequest*)request if implemented
					//Notes, the WebApplicationController, should initialize and setup the controller's views, and other properties
					//here if a custom (not defualt view) is to be used. Initializeing this method will cause the WebApplicationController
					//to not prepare any defualts based on the request.
				6. Else a class with name <ActionName><ControllerName>View is looked up and initialized if found.
				7. WebApplicationController calls - (id)<actionName>Action:(WebRequest*)request
				8. If the above call returns a WebResponse object, then it is rendered, else...
				9. A WebResponse is returned from [controller.view displayWithData:(above response)] //typically a NSDictionary
				10. WebApplicationController calls - (void)postProcessResponse:(FOWResponse*)response fromRequest:(WebRequest*)request; on controller
				11. if controller provides "components" array, call -postPRocessResponse for each while result is true (false returns a notFound response)
 
 //TODO: fix documentation for WebLayoutViews

				1. WebViews handleing rendering in the following flow.
				- (WebResponse*)displayWithData:(NSDictionary*)data is called.
				//Subclasses can overide this to provide complete custom rendering.
				2. The defualt implementation finds a <ClassName>Template.<extention> template file using 
				- (NSData*)templateData;
				3. The defualt implementation then parses the template with
				- (NSData*)processedTemplateData:(NSData*)templateData;
				4. The defualt implementation then converts the template data into a WebResponse with
				- (WebResponse*)responseForProcessedTemplate:(NSData*)template;
 */
@protocol WebActionController <NSObject>
@property (nonatomic, retain) WebActionView* view;

/*!
	If this returns nil, the the base layout view is returned. Controllers that with to provide custom layout controllers
	can subclass WebLayoutView and provide that object in the - init<Action>Action method
 */
@property (nonatomic, retain) WebLayoutView* layout;

@optional
/*!
	\brief	An ordered array of component names that are inserted into the request graph for processing.
	\detail	See WebComponent header for implementation details.
 */
 - (NSArray*)components;
 
/*!
	\brief	Gives the controller a per/request ability to set component properties.
	\detail	This gets called prior to -preProcessRequest, and can be used for
				component variable configureation specific to a given controller
				
				IE the Auth component uses this to allow the controller to dynamically set the
				allowed/deny properites for actions outside of its global AuthComponent.plist
 */
- (NSDictionary*)prepareComponentWithName:(NSString*)componentName;

/*
	Optional
	Useing this an action controller can use alternate action methods then the typical built in
	auto method-selecto resolving. This gets called first (if implemented) if no result, then
	a regular selector method is called
 */
- (SEL)selectorForActionName:(NSString*)name;
- (void)preProcessRequest:(WebRequest*)request;

/*
	Optional
	Controllers can use this to post process the response before it hits the renderer.
	The response could be, nil, WebResponse, NSString or more typically data for the
	rendered to used in rendering layout.
 
	Controllers can return a new response if they with to modify the data.
 */
- (id)postProcessResponse:(id)response fromRequest:(WebRequest*)request;

- (id)defualtAction:(WebRequest*)request;

@end
