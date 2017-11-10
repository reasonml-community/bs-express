type done_;


/*** abstract type which ensure middleware function must either
     call the [next] function or one of the [send] function on the
     response object.

     This should be a great argument for OCaml, the type system
     prevents silly error which in this case would make the server hang */

/*** TODO : maybe this should be a more common module like bs-error */
module Error = {
  type t;

  /*** Error type */
  [@bs.send] [@bs.return null_undefined_to_opt] external message : t => option(string) = "";
  [@bs.send] [@bs.return null_undefined_to_opt] external name : t => option(string) = "";
};

module Request = {
  type t;
  type params = Js_dict.t(Js_json.t);
  [@bs.get] external params : t => params = "";

  /*** [params request] return the JSON object filled with the
       request parameters */
  external asJsonObject : t => Js_dict.t(Js_json.t) = "%identity";

  /*** [asJsonObject request] casts a [request] to a JSON object. It is
       common in Express application to use the Request object as a
       placeholder to maintain state through the various middleware which
       are executed. */
  [@bs.get] external baseUrl : t => string = "";

  /*** [baseUrl request] returns the 'baseUrl' property */
  [@bs.get] [@bs.return nullable] external bodyJSON : t => option(Js.Json.t) = "body";

  /*** When using the json body-parser middleware and receiving a request with a
       content type of "application/json", this property is a Js.Json.t that
       contains the body sent by the request. **/
  [@bs.get] [@bs.return nullable] external bodyRaw : t => option(Node_buffer.t) = "body";

  /*** When using the raw body-parser middleware and receiving a request with a
       content type of "application/octet-stream", this property is a
       Node_buffer.t that contains the body sent by the request. **/
  [@bs.get] [@bs.return nullable] external bodyText : t => option(string) = "body";

  /*** When using the text body-parser middleware and receiving a request with a
       content type of "text/plain", this property is a string that
       contains the body sent by the request. **/
  [@bs.get] [@bs.return nullable] external bodyURLEncoded : t => option(Js.Dict.t(string)) =
    "body";

  /*** When using the urlencoded body-parser middleware and receiving a request
       with a content type of "application/x-www-form-urlencoded", this property
       is a Js.Dict.t string that contains the body sent by the request. **/
  [@bs.get] [@bs.return null_undefined_to_opt]
  external cookies : t => option(Js.Dict.t(Js.Json.t)) =
    "";

  /*** When using cookie-parser middleware, this property is an object
       that contains cookies sent by the request. If the request contains
       no cookies, it defaults to {}.*/
  [@bs.get] [@bs.return null_undefined_to_opt]
  external signedCookies : t => option(Js.Dict.t(Js.Json.t)) =
    "";

  /*** When using cookie-parser middleware, this property contains signed cookies
       sent by the request, unsigned and ready for use. Signed cookies reside in
       a different object to show developer intent; otherwise, a malicious attack
       could be placed on req.cookie values (which are easy to spoof).
       Note that signing a cookie does not make it “hidden” or encrypted;
       but simply prevents tampering (because the secret used to
       sign is private). **/
  [@bs.get] external hostname : t => string = "";

  /*** [hostname request] Contains the hostname derived from the Host
       HTTP header.*/
  [@bs.get] external ip : t => string = "";

  /*** [ip request] Contains the remote IP address of the request.*/
  [@bs.get] external fresh : t => bool = "";

  /*** [fresh request] returns [true] whether the request is "fresh" */
  [@bs.get] external stale : t => bool = "";

  /*** [stale request] returns [true] whether the request is "stale"*/
  [@bs.get] external methodRaw : t => string = "method";
  type method_ =
    | Get
    | Post
    | Put
    | Delete
    | Head
    | Options
    | Trace
    | Connect;
  let method: t => method_ =
    (req) =>
      switch (methodRaw(req)) {
      | "GET" => Get
      | "POST" => Post
      | "PUT" => Put
      | "DELETE" => Delete
      | "HEAD" => Head
      | "OPTIONS" => Options
      | "TRACE" => Trace
      | "CONNECT" => Connect
      | s => failwith("Express.Request.method_ Unexpected method: " ++ s)
      };

  /*** [method_ request] return a string corresponding to the HTTP
       method of the request: GET, POST, PUT, and so on */
  [@bs.get] external originalUrl : t => string = "";

  /*** [originalUrl request] returns the original url. See
       https://expressjs.com/en/4x/api.html#req.originalUrl */
  [@bs.get] external path : t => string = "";

  /*** [path request] returns the path part of the request URL.*/
  type protocol =
    | Http
    | Https;
  let protocol: t => protocol =
    (req) => {
      module Raw = {
        [@bs.get] external protocol : t => string = "protocol";
      };
      switch (Raw.protocol(req)) {
      | "http" => Http
      | "https" => Https
      | s => failwith("Express.Request.protocol, Unexpected protocol: " ++ s)
      }
    };

  /*** [protocol request] returns the request protocol string: either http
       or (for TLS requests) https. */
  [@bs.get] external secure : t => bool = "";

  /*** [secure request] returns [true] if a TLS connection is established */
  [@bs.get] external query : t => Js.Dict.t(Js.Json.t) = "";

  /*** [query request] returns an object containing a property for each
       query string parameter in the route. If there is no query string,
       it returns the empty object, {} */
  let accepts: (t, array(string)) => option(string) =
    (req, types) => {
      module Raw = {
        [@bs.send] external accepts : (t, array(string)) => Js.Json.t = "accepts";
      };
      let ret = Raw.accepts(req, types);
      let tagged_t = Js_json.classify(ret);
      switch tagged_t {
      | JSONString(x) => Some(x)
      | _ => None
      }
    };

  /*** [acceptsRaw accepts types] checks if the specified content types
       are acceptable, based on the request's Accept HTTP header field.
       The method returns the best match, or if none of the specified
       content types is acceptable, returns [false] */
  let acceptsCharsets: (t, array(string)) => option(string) =
    (req, types) => {
      module Raw = {
        [@bs.send] external acceptsCharsets : (t, array(string)) => Js.Json.t = "acceptsCharsets";
      };
      let ret = Raw.acceptsCharsets(req, types);
      let tagged_t = Js_json.classify(ret);
      switch tagged_t {
      | JSONString(x) => Some(x)
      | _ => None
      }
    };
  [@bs.send] [@bs.return null_undefined_to_opt] external get : (t, string) => option(string) = "";

  /*** [get return field] returns the specified HTTP request header
       field (case-insensitive match) */
  [@bs.get] external xhr : t => bool = "";
  /*** [xhr request] returns [true] if the request’s X-Requested-With
       header field is "XMLHttpRequest", indicating that the request was
       issued by a client library such as jQuery */
};

module Response = {
  type t;
  [@bs.send] external sendFile : (t, string, 'a) => done_ = "";
  [@bs.send] external sendString : (t, string) => done_ = "send";
  [@bs.send] external sendJson : (t, Js.Json.t) => done_ = "json";
  [@bs.send] external sendBuffer : (t, Buffer.t) => done_ = "send";
  [@bs.send] external sendArray : (t, array('a)) => done_ = "send";
  [@bs.send] [@ocaml.deprecated "Use sendJson instead`"] external json : (t, Js.Json.t) => done_ =
    "";
  [@bs.send] external redirectCode : (t, int, string) => done_ = "redirect";
  [@bs.send] external redirect : (t, string) => done_ = "redirect";
};

module Next: {
  type content;
  type t = Js.undefined(content) => done_;
  let middleware: Js.undefined(content);

  /*** value to use as [next] callback argument to invoke the next
       middleware */
  let route: Js.undefined(content);

  /*** value to use as [next] callback argument to skip middleware
       processing for the current route.*/
  let error: Error.t => Js.undefined(content);
  /*** [error e] returns the argument for [next] callback to be propagate
       error [e] through the chain of middleware. */
} = {
  type content;
  type t = Js.undefined(content) => done_;
  let middleware = Js.undefined;
  external castToContent : 'a => content = "%identity";
  let route = Js.Undefined.return(castToContent("route"));
  let error = (e: Error.t) => Js.Undefined.return(castToContent(e));
};

module Middleware = {
  type next = Next.t;
  type t;
  /* Middleware abstract type which unified the various way an
     Express middleware can be constructed:
     {ul
     {- from a {b function} using the [ofFunction]}
     {- from a Router class}
     {- from an App class}
     {- from the third party middleware modules}
     }
     For each of the class which implements the middleware interface
     is JavaScript, one can use the "%identity" function to cast
     it to this type [t] */
  type f = (Request.t, Response.t, next) => done_;
  external from : f => t = "%identity";

  /*** [from f] creates a Middleware from a function */
  type errorF = (Error.t, Request.t, Response.t, next) => done_;
  external fromError : errorF => t = "%identity";
  /*** [fromError f] creates a Middleware from an error function */
};

/* Generate the common Middleware binding function for a given
 * type. This Functor is used for the Router and App classes. */
module MakeBindFunctions = (T: {type t;}) => {
  [@bs.send] external use : (T.t, Middleware.t) => unit = "";
  [@bs.send] external useWithMany : (T.t, array(Middleware.t)) => unit = "use";
  [@bs.send] external useOnPath : (T.t, ~path: string, Middleware.t) => unit = "use";
  [@bs.send] external useOnPathWithMany : (T.t, ~path: string, array(Middleware.t)) => unit =
    "use";
  [@bs.send] external get : (T.t, ~path: string, Middleware.t) => unit = "";
  [@bs.send] external getWithMany : (T.t, ~path: string, array(Middleware.t)) => unit = "get";
  [@bs.send] external post : (T.t, ~path: string, Middleware.t) => unit = "";
  [@bs.send] external postWithMany : (T.t, ~path: string, array(Middleware.t)) => unit = "post";
};

module App = {
  type t;
  include
    MakeBindFunctions(
      {
        type nonrec t = t;
      }
    );
  [@bs.module] external make : unit => t = "express";

  /*** [make ()] creates an instance of the App class. */
  external asMiddleware : t => Middleware.t = "%identity";

  /*** [asMiddleware app] casts an App instance to a Middleware type */
  [@bs.send]
  external listen_ : (t, int, [@bs.uncurry] (Js.Null_undefined.t(Js.Exn.t) => unit)) => unit =
    "listen";
  let listen = (app, ~port=3000, ~onListen=(_) => (), ()) => listen_(app, port, onListen);
};

let express = App.make;


/*** [express ()] creates an instance of the App class.
     Alias for [App.make ()] */
module Static = {
  type options;
  let defaultOptions: unit => options = fun () => (Obj.magic(Js_obj.empty()): options);
  [@bs.set] external dotfiles : (options, string) => unit = "";
  [@bs.set] external etag : (options, Js.boolean) => unit = "";
  /* ... add all the other options */
  type t;
  [@bs.module "express"] external make : (string, options) => t = "static";

  /*** [make directory] creates a static middleware for [directory] */
  external asMiddleware : t => Middleware.t = "%identity";
  /*** [asMiddleware static] casts [static] to a Middleware type */
};
