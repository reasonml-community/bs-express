type complete;


/*** abstract type which ensure middleware function must either
     call the [next] function or one of the [send] function on the
     response object.

     This should be a great argument for OCaml, the type system
     prevents silly error which in this case would make the server hang */
module Error: {
  type t = exn;

  /*** Error type */
  let message: Js_exn.t => option(string);
  let name: Js_exn.t => option(string);
};

module Request: {
  type t;
  type params = Js.Dict.t(Js.Json.t);

  /*** [params request] return the JSON object filled with the
       request parameters */
  let params: t => params;

  /*** [asJsonObject request] casts a [request] to a JSON object. It is
       common in Express application to use the Request object as a
       placeholder to maintain state through the various middleware which
       are executed. */
  let asJsonObject: t => Js.Dict.t(Js.Json.t);

  /*** [baseUrl request] returns the 'baseUrl' property */
  let baseUrl: t => string;

  /*** When using the json body-parser middleware and receiving a request with a
       content type of "application/json", this property is a Js.Json.t that
       contains the body sent by the request. **/
  let bodyJSON: t => option(Js.Json.t);

  /*** When using the raw body-parser middleware and receiving a request with a
       content type of "application/octet-stream", this property is a
       Node_buffer.t that contains the body sent by the request. **/
  let bodyRaw: t => option(Node_buffer.t);

  /*** When using the text body-parser middleware and receiving a request with a
       content type of "text/plain", this property is a string that
       contains the body sent by the request. **/
  let bodyText: t => option(string);

  /*** When using the urlencoded body-parser middleware and receiving a request
       with a content type of "application/x-www-form-urlencoded", this property
        is a Js.Dict.t string that contains the body sent by the request. **/
  let bodyURLEncoded: t => option(Js.Dict.t(string));

  /*** When using cookie-parser middleware, this property is an object
       that contains cookies sent by the request. If the request contains
       no cookies, it defaults to {}.*/
  let cookies: t => option(Js.Dict.t(Js.Json.t));

  /*** When using cookie-parser middleware, this property contains signed cookies
       sent by the request, unsigned and ready for use. Signed cookies reside in
       a different object to show developer intent; otherwise, a malicious attack
       could be placed on req.cookie values (which are easy to spoof).
       Note that signing a cookie does not make it “hidden” or encrypted;
       but simply prevents tampering (because the secret used to
       sign is private). **/
  let signedCookies: t => option(Js.Dict.t(Js.Json.t));

  /*** [hostname request] Contains the hostname derived from the Host
       HTTP header.*/
  let hostname: t => string;

  /*** [ip request] Contains the remote IP address of the request.*/
  let ip: t => string;

  /*** [fresh request] returns [true] whether the request is "fresh" */
  let fresh: t => bool;

  /*** [stale request] returns [true] whether the request is "stale"*/
  let stale: t => bool;

  /*** [method_ request] return a string corresponding to the HTTP
       method of the request: GET, POST, PUT, and so on */
  let methodRaw: t => string;
  type httpMethod =
    | Get
    | Post
    | Put
    | Delete
    | Head
    | Options
    | Trace
    | Connect
    | Patch;

  /*** [method_ request] return a variant corresponding to the HTTP
       method of the request: Get, Post, Put, and so on */
  let httpMethod: t => httpMethod;

  /*** [originalUrl request] returns the original url. See
       https://expressjs.com/en/4x/api.html#req.originalUrl */
  let originalUrl: t => string;

  /*** [path request] returns the path part of the request URL.*/
  let path: t => string;
  type protocol =
    | Http
    | Https;

  /*** [protocol request] returns the request protocol string: either http
       or (for TLS requests) https. */
  let protocol: t => protocol;

  /*** [secure request] returns [true] if a TLS connection is established */
  let secure: t => bool;

  /*** [query request] returns an object containing a property for each
       query string parameter in the route. If there is no query string,
       it returns the empty object, {} */
  let query: t => Js.Dict.t(Js.Json.t);

  /*** [acceptsRaw accepts types] checks if the specified content types
       are acceptable, based on the request's Accept HTTP header field.
       The method returns the best match, or if none of the specified
       content types is acceptable, returns [false] */
  let accepts: (array(string), t) => option(string);
  let acceptsCharsets: (array(string), t) => option(string);

  /*** [get return field] returns the specified HTTP request header
       field (case-insensitive match) */
  let get: (string, t) => option(string);

  /*** [xhr request] returns [true] if the request’s X-Requested-With
       header field is "XMLHttpRequest", indicating that the request was
       issued by a client library such as jQuery */
  let xhr: t => bool;
};

module Response: {
  type t;
  module StatusCode: {
    type t =
      | Ok
      | Created
      | Accepted
      | NonAuthoritativeInformation
      | NoContent
      | ResetContent
      | PartialContent
      | MultiStatus
      | AleadyReported
      | IMUsed
      | MultipleChoices
      | MovedPermanently
      | Found
      | SeeOther
      | NotModified
      | UseProxy
      | SwitchProxy
      | TemporaryRedirect
      | PermanentRedirect
      | BadRequest
      | Unauthorized
      | PaymentRequired
      | Forbidden
      | NotFound
      | MethodNotAllowed
      | NotAcceptable
      | ProxyAuthenticationRequired
      | RequestTimeout
      | Conflict
      | Gone
      | LengthRequired
      | PreconditionFailed
      | PayloadTooLarge
      | UriTooLong
      | UnsupportedMediaType
      | RangeNotSatisfiable
      | ExpectationFailed
      | ImATeapot
      | MisdirectedRequest
      | UnprocessableEntity
      | Locked
      | FailedDependency
      | UpgradeRequired
      | PreconditionRequired
      | TooManyRequests
      | RequestHeaderFieldsTooLarge
      | UnavailableForLegalReasons
      | InternalServerError
      | NotImplemented
      | BadGateway
      | ServiceUnavailable
      | GatewayTimeout
      | HttpVersionNotSupported
      | VariantAlsoNegotiates
      | InsufficientStorage
      | LoopDetected
      | NotExtended
      | NetworkAuthenticationRequired;
    let fromInt: int => option(t);
    let toInt: t => int;
  };
  let sendFile: (string, 'a, t) => complete;
  let sendString: (string, t) => complete;
  let sendJson: (Js.Json.t, t) => complete;
  let sendBuffer: (Node.Buffer.t, t) => complete;
  let sendArray: (array('a), t) => complete;
  let sendRawStatus: (int, t) => complete;
  let sendStatus: (StatusCode.t, t) => complete;
  let rawStatus: (int, t) => t;
  let status: (StatusCode.t, t) => t;
  let cookie:
    (
      ~name: string,
      ~maxAge: int=?,
      ~expiresGMT: Js.Date.t=?,
      ~httpOnly: bool=?,
      ~secure: bool=?,
      ~signed: bool=?,
      ~path: string=?,
      ~sameSite: [ | `Lax | `Strict]=?,
      Js.Json.t,
      t
    ) =>
    t;

  /***
   Web browsers and other compliant clients will only clear the cookie if the given options is identical to those given to res.cookie(), excluding expires and maxAge.
    */
  let clearCookie:
    (
      ~name: string,
      ~httpOnly: bool=?,
      ~secure: bool=?,
      ~signed: bool=?,
      ~path: string=?,
      ~sameSite: [ | `Lax | `Strict]=?,
      t
    ) =>
    t;
  let json: (Js.Json.t, t) => complete;
  let redirectCode: (int, string, t) => complete;
  let redirect: (string, t) => complete;
  let setHeader: (string, string, t) => t;
  let setLinks: (Js.Dict.t(string), t) => t;
};

module Next: {
  type content;
  type t = (Js.undefined(content), Response.t) => complete;

  /*** value to use as [next] callback argument to invoke the next
       middleware */
  let middleware: Js.undefined(content);

  /*** value to use as [next] callback argument to skip middleware
       processing for the current route.*/
  let route: Js.undefined(content);

  /*** [error e] returns the argument for [next] callback to be propagate
       error [e] through the chain of middleware. */
  let error: Error.t => Js.undefined(content);
};

module ByteLimit: {
  type t =
    | B(int)
    | Kb(float)
    | Mb(float)
    | Gb(float);
  let b: int => t;
  let kb: float => t;
  let mb: float => t;
  let gb: float => t;
};

module Middleware: {
  type t;
  type next = Next.t;
  let json:
    (~inflate: bool=?, ~strict: bool=?, ~limit: ByteLimit.t=?, unit) => t;
  let text:
    (
      ~defaultCharset: string=?,
      ~fileType: string=?,
      ~inflate: bool=?,
      ~limit: ByteLimit.t=?,
      unit
    ) =>
    t;
  let raw:
    (~inflate: bool=?, ~fileType: string=?, ~limit: ByteLimit.t=?, unit) => t;
  let urlencoded:
    (
      ~extended: bool=?,
      ~inflate: bool=?,
      ~limit: ByteLimit.t=?,
      ~parameterLimit: int=?,
      unit
    ) =>
    t;
  module type S = {
    type f;
    let from: f => t;
    type errorF;
    let fromError: errorF => t;
  };
  module type ApplyMiddleware = {
    type f;
    let apply: (f, next, Request.t, Response.t) => unit;
    type errorF;
    let applyWithError: (errorF, next, Error.t, Request.t, Response.t) => unit;
  };
  module Make:
    (A: ApplyMiddleware) => S with type f = A.f and type errorF = A.errorF;
  include
    S with
      type f = (next, Request.t, Response.t) => complete and
      type errorF = (next, Error.t, Request.t, Response.t) => complete;
};

module PromiseMiddleware:
  Middleware.S with
    type f =
      (Middleware.next, Request.t, Response.t) => Js.Promise.t(complete) and
    type errorF =
      (Middleware.next, Error.t, Request.t, Response.t) =>
      Js.Promise.t(complete);

module type Routable = {
  type t;
  let use: (t, Middleware.t) => unit;
  let useWithMany: (t, array(Middleware.t)) => unit;
  let useOnPath: (t, ~path: string, Middleware.t) => unit;
  let useOnManyPaths: (t, ~paths: array(string), Middleware.t) => unit;
  let useOnPathWithMany: (t, ~path: string, array(Middleware.t)) => unit;
  let get: (t, ~path: string, Middleware.t) => unit;
  let getOnManyPaths: (t, ~paths: array(string), Middleware.t) => unit;
  let getWithMany: (t, ~path: string, array(Middleware.t)) => unit;
  let param: (t, ~name: string, Middleware.t) => unit;
  let post: (t, ~path: string, Middleware.t) => unit;
  let postOnManyPaths: (t, ~paths: array(string), Middleware.t) => unit;
  let postWithMany: (t, ~path: string, array(Middleware.t)) => unit;
  let put: (t, ~path: string, Middleware.t) => unit;
  let putOnManyPaths: (t, ~paths: array(string), Middleware.t) => unit;
  let putWithMany: (t, ~path: string, array(Middleware.t)) => unit;
  let patch: (t, ~path: string, Middleware.t) => unit;
  let patchWithMany: (t, ~path: string, array(Middleware.t)) => unit;
  let delete: (t, ~path: string, Middleware.t) => unit;
  let deleteWithMany: (t, ~path: string, array(Middleware.t)) => unit;
};

module MakeBindFunctions: (T: {type t;}) => Routable with type t = T.t;

module Router: {
  include Routable;
  let make:
    (~caseSensitive: bool=?, ~mergeParams: bool=?, ~strict: bool=?, unit) => t;
  let asMiddleware: t => Middleware.t;
};

module HttpServer: {type t;};

let router:
  (~caseSensitive: bool=?, ~mergeParams: bool=?, ~strict: bool=?, unit) =>
  Router.t;

module App: {
  include Routable;
  let make: unit => t;
  let asMiddleware: t => Middleware.t;
  let useRouter: (t, Router.t) => unit;
  let useRouterOnPath: (t, ~path: string, Router.t) => unit;
  let listen:
    (
      t,
      ~port: int=?,
      ~onListen: Js.null_undefined(Js.Exn.t) => unit=?,
      unit
    ) =>
    HttpServer.t;
};


/*** [express ()] creates an instance of the App class.
     Alias for [App.make ()] */
let express: unit => App.t;

module Static: {
  type options;
  type stat;
  let defaultOptions: unit => options;
  let dotfiles: (options, string) => unit;
  let etag: (options, bool) => unit;
  let extensions: (options, array(string)) => unit;
  let fallthrough: (options, bool) => unit;
  let immutable: (options, bool) => unit;
  let indexBool: (options, bool) => unit;
  let indexString: (options, string) => unit;
  let lastModified: (options, bool) => unit;
  let maxAge: (options, int) => unit;
  let redirect: (options, bool) => unit;
  let setHeaders: (options, (Request.t, string, stat) => unit) => unit;
  type t;

  /*** [make directory] creates a static middleware for [directory] */
  let make: (string, options) => t;

  /*** [asMiddleware static] casts [static] to a Middleware type */
  let asMiddleware: t => Middleware.t;
};
