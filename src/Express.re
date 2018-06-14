type complete;

module Error = {
  type t = exn;

  /*** Error type */
  [@bs.send] [@bs.return null_undefined_to_opt]
  external message : Js_exn.t => option(string) = "";
  [@bs.send] [@bs.return null_undefined_to_opt]
  external name : Js_exn.t => option(string) = "";
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
  [@bs.get] external body_ : t => 'a = "body";

  /*** When using the json body-parser middleware and receiving a request with a
       content type of "application/json", this property is a Js.Json.t that
       contains the body sent by the request. **/
  [@bs.get] [@bs.return null_undefined_to_opt]
  external bodyJSON : t => option(Js.Json.t) = "body";

  /*** When using the raw body-parser middleware and receiving a request with a
       content type of "application/octet-stream", this property is a
       Node_buffer.t that contains the body sent by the request. **/
  [@bs.get] [@bs.return null_undefined_to_opt]
  external bodyRaw : t => option(Node.Buffer.t) = "body";

  /*** When using the text body-parser middleware and receiving a request with a
       content type of "text/plain", this property is a string that
       contains the body sent by the request. **/
  let bodyText: t => option(string) =
    req => {
      let body: string = body_(req);
      if (Js.Json.test(body, Js.Json.String)) {
        Some(body);
      } else {
        None;
      };
    };
  let decodeStringDict = json =>
    Js.Json.decodeObject(json)
    |> Js.Option.andThen((. obj) => {
         let source: Js.Dict.t(string) = Obj.magic(obj);
         let allStrings =
           Js.Dict.values(source)
           |> Array.fold_left(
                (prev, value) => prev && Js.Json.test(value, Js.Json.String),
                true,
              );
         if (allStrings) {
           Some(source);
         } else {
           None;
         };
       });
  let bodyURLEncoded: t => option(Js.Dict.t(string)) =
    req => {
      let body: Js.Json.t = body_(req);
      decodeStringDict(body);
    };

  /*** When using the urlencoded body-parser middleware and receiving a request
       with a content type of "application/x-www-form-urlencoded", this property
       is a Js.Dict.t string that contains the body sent by the request. **/
  [@bs.get] [@bs.return null_undefined_to_opt]
  external cookies : t => option(Js.Dict.t(Js.Json.t)) = "";

  /*** When using cookie-parser middleware, this property is an object
       that contains cookies sent by the request. If the request contains
       no cookies, it defaults to {}.*/
  [@bs.get] [@bs.return null_undefined_to_opt]
  external signedCookies : t => option(Js.Dict.t(Js.Json.t)) = "";

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
  let httpMethod: t => httpMethod =
    req =>
      switch (methodRaw(req)) {
      | "GET" => Get
      | "POST" => Post
      | "PUT" => Put
      | "PATCH" => Patch
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
    req => {
      module Raw = {
        [@bs.get] external protocol : t => string = "protocol";
      };
      switch (Raw.protocol(req)) {
      | "http" => Http
      | "https" => Https
      | s => failwith("Express.Request.protocol, Unexpected protocol: " ++ s)
      };
    };

  /*** [protocol request] returns the request protocol string: either http
       or (for TLS requests) https. */
  [@bs.get] external secure : t => bool = "";

  /*** [secure request] returns [true] if a TLS connection is established */
  [@bs.get] external query : t => Js.Dict.t(Js.Json.t) = "";

  /*** [query request] returns an object containing a property for each
       query string parameter in the route. If there is no query string,
       it returns the empty object, {} */
  let accepts: (array(string), t) => option(string) =
    (types, req) => {
      module Raw = {
        [@bs.send]
        external accepts : (t, array(string)) => Js.Json.t = "accepts";
      };
      let ret = Raw.accepts(req, types);
      let tagged_t = Js_json.classify(ret);
      switch (tagged_t) {
      | JSONString(x) => Some(x)
      | _ => None
      };
    };

  /*** [acceptsRaw accepts types] checks if the specified content types
       are acceptable, based on the request's Accept HTTP header field.
       The method returns the best match, or if none of the specified
       content types is acceptable, returns [false] */
  let acceptsCharsets: (array(string), t) => option(string) =
    (types, req) => {
      module Raw = {
        [@bs.send]
        external acceptsCharsets : (t, array(string)) => Js.Json.t =
          "acceptsCharsets";
      };
      let ret = Raw.acceptsCharsets(req, types);
      let tagged_t = Js_json.classify(ret);
      switch (tagged_t) {
      | JSONString(x) => Some(x)
      | _ => None
      };
    };
  [@bs.send.pipe: t] [@bs.return null_undefined_to_opt]
  external get : string => option(string) = "";

  /*** [get return field] returns the specified HTTP request header
       field (case-insensitive match) */
  [@bs.get] external xhr : t => bool = "";
  /*** [xhr request] returns [true] if the request’s X-Requested-With
       header field is "XMLHttpRequest", indicating that the request was
       issued by a client library such as jQuery */
};

module Response = {
  type t;
  module StatusCode = {
    [@bs.deriving jsConverter]
    type t =
      | [@bs.as 200] Ok
      | [@bs.as 201] Created
      | [@bs.as 202] Accepted
      | [@bs.as 203] NonAuthoritativeInformation
      | [@bs.as 204] NoContent
      | [@bs.as 205] ResetContent
      | [@bs.as 206] PartialContent
      | [@bs.as 207] MultiStatus
      | [@bs.as 208] AleadyReported
      | [@bs.as 226] IMUsed
      | [@bs.as 300] MultipleChoices
      | [@bs.as 301] MovedPermanently
      | [@bs.as 302] Found
      | [@bs.as 303] SeeOther
      | [@bs.as 304] NotModified
      | [@bs.as 305] UseProxy
      | [@bs.as 306] SwitchProxy
      | [@bs.as 307] TemporaryRedirect
      | [@bs.as 308] PermanentRedirect
      | [@bs.as 400] BadRequest
      | [@bs.as 401] Unauthorized
      | [@bs.as 402] PaymentRequired
      | [@bs.as 403] Forbidden
      | [@bs.as 404] NotFound
      | [@bs.as 405] MethodNotAllowed
      | [@bs.as 406] NotAcceptable
      | [@bs.as 407] ProxyAuthenticationRequired
      | [@bs.as 408] RequestTimeout
      | [@bs.as 409] Conflict
      | [@bs.as 410] Gone
      | [@bs.as 411] LengthRequired
      | [@bs.as 412] PreconditionFailed
      | [@bs.as 413] PayloadTooLarge
      | [@bs.as 414] UriTooLong
      | [@bs.as 415] UnsupportedMediaType
      | [@bs.as 416] RangeNotSatisfiable
      | [@bs.as 417] ExpectationFailed
      | [@bs.as 418] ImATeapot
      | [@bs.as 421] MisdirectedRequest
      | [@bs.as 422] UnprocessableEntity
      | [@bs.as 423] Locked
      | [@bs.as 424] FailedDependency
      | [@bs.as 426] UpgradeRequired
      | [@bs.as 428] PreconditionRequired
      | [@bs.as 429] TooManyRequests
      | [@bs.as 431] RequestHeaderFieldsTooLarge
      | [@bs.as 451] UnavailableForLegalReasons
      | [@bs.as 500] InternalServerError
      | [@bs.as 501] NotImplemented
      | [@bs.as 502] BadGateway
      | [@bs.as 503] ServiceUnavailable
      | [@bs.as 504] GatewayTimeout
      | [@bs.as 505] HttpVersionNotSupported
      | [@bs.as 506] VariantAlsoNegotiates
      | [@bs.as 507] InsufficientStorage
      | [@bs.as 508] LoopDetected
      | [@bs.as 510] NotExtended
      | [@bs.as 511] NetworkAuthenticationRequired;
    let fromInt = tFromJs;
    let toInt = tToJs;
  };
  [@bs.send.pipe: t]
  external cookie_ : (string, Js.Json.t, 'a) => unit = "cookie";
  [@bs.send.pipe: t]
  external clearCookie_ : (string, 'a) => unit = "clearCookie";
  [@bs.deriving jsConverter]
  type sameSite = [ | [@bs.as "lax"] `Lax | [@bs.as "strict"] `Strict];
  external toDict : 'a => Js.Dict.t(Js.nullable('b)) = "%identity";
  let filterKeys = obj => {
    let result = toDict(obj);
    result
    |> Js.Dict.entries
    |> Js.Array.filter(((_key, value)) => ! Js.Nullable.test(value))
    |> Js.Dict.fromArray;
  };
  let cookie =
      (
        ~name,
        ~maxAge=?,
        ~expiresGMT=?,
        ~httpOnly=?,
        ~secure=?,
        ~signed=?,
        ~path=?,
        ~sameSite: option(sameSite)=?,
        value,
        response,
      ) => {
    cookie_(
      name,
      value,
      {
        "maxAge": maxAge |> Js.Nullable.fromOption,
        "expires": expiresGMT |> Js.Nullable.fromOption,
        "path": path |> Js.Nullable.fromOption,
        "httpOnly": httpOnly |> Js.Nullable.fromOption,
        "secure": secure |> Js.Nullable.fromOption,
        "sameSite":
          sameSite
          |> Js.Option.map((. x) => sameSiteToJs(x))
          |> Js.Nullable.fromOption,
        "signed": signed |> Js.Nullable.fromOption,
      }
      |> filterKeys,
      response,
    );
    response;
  };
  let clearCookie =
      (
        ~name,
        ~httpOnly=?,
        ~secure=?,
        ~signed=?,
        ~path="/",
        ~sameSite: option(sameSite)=?,
        response,
      ) => {
    clearCookie_(
      name,
      {
        "maxAge": Js.Nullable.undefined,
        "expires": Js.Nullable.undefined,
        "path": path,
        "httpOnly": httpOnly |> Js.Nullable.fromOption,
        "secure": secure |> Js.Nullable.fromOption,
        "sameSite":
          sameSite
          |> Js.Option.map((. x) => sameSiteToJs(x))
          |> Js.Nullable.fromOption,
        "signed": signed |> Js.Nullable.fromOption,
      }
      |> filterKeys,
      response,
    );
    response;
  };
  [@bs.send.pipe: t] external sendFile : (string, 'a) => complete = "";
  [@bs.send.pipe: t] external sendString : string => complete = "send";
  [@bs.send.pipe: t] external sendJson : Js.Json.t => complete = "json";
  [@bs.send.pipe: t] external sendBuffer : Node.Buffer.t => complete = "send";
  [@bs.send.pipe: t] external sendArray : array('a) => complete = "send";
  [@bs.send.pipe: t] external sendRawStatus : int => complete = "sendStatus";
  let sendStatus = statusCode => sendRawStatus(StatusCode.toInt(statusCode));
  [@bs.send.pipe: t] external rawStatus : int => t = "status";
  let status = statusCode => rawStatus(StatusCode.toInt(statusCode));
  [@bs.send.pipe: t] [@ocaml.deprecated "Use sendJson instead`"]
  external json : Js.Json.t => complete = "";
  [@bs.send.pipe: t]
  external redirectCode : (int, string) => complete = "redirect";
  [@bs.send.pipe: t] external redirect : string => complete = "redirect";
  [@bs.send.pipe: t] external setHeader : (string, string) => t = "set";
  [@bs.send.pipe: t] external setLinks : Js.Dict.t(string) => t = "links";
};

module Next: {
  type content;
  type t = (Js.undefined(content), Response.t) => complete;
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
  type t = (Js.undefined(content), Response.t) => complete;
  let middleware = Js.undefined;
  external castToContent : 'a => content = "%identity";
  let route = Js.Undefined.return(castToContent("route"));
  let error = (e: Error.t) => Js.Undefined.return(castToContent(e));
};

module ByteLimit = {
  [@bs.deriving accessors]
  type t =
    | B(int)
    | Kb(float)
    | Mb(float)
    | Gb(float);
  let toBytes =
    fun
    | Some(B(b)) => Js.Nullable.return(b)
    | Some(Kb(kb)) => Js.Nullable.return(int_of_float(1024.0 *. kb))
    | Some(Mb(mb)) =>
      Js.Nullable.return(int_of_float(1024.0 *. 1024.0 *. mb))
    | Some(Gb(gb)) =>
      Js.Nullable.return(int_of_float(1024.0 *. 1024.0 *. 1024.0 *. gb))
    | None => Js.Nullable.undefined;
};

module Middleware = {
  type next = Next.t;
  type t;
  type jsonOptions = {
    .
    "inflate": bool,
    "strict": bool,
    "limit": Js.nullable(int),
  };
  type urlEncodedOptions = {
    .
    "extended": bool,
    "inflate": bool,
    "limit": Js.nullable(int),
    "parameterLimit": Js.nullable(int),
  };
  type textOptions = {
    .
    "defaultCharset": string,
    "inflate": bool,
    "type": string,
    "limit": Js.Nullable.t(int),
  };
  type rawOptions = {
    .
    "inflate": bool,
    "type": string,
    "limit": Js.Nullable.t(int),
  };
  [@bs.module "express"] [@bs.val] external json_ : jsonOptions => t = "json";
  [@bs.module "express"] [@bs.val]
  external urlencoded_ : urlEncodedOptions => t = "urlencoded";
  let json = (~inflate=true, ~strict=true, ~limit=?, ()) =>
    json_({
      "inflate": inflate,
      "strict": strict,
      "limit": ByteLimit.toBytes(limit),
    });
  [@bs.module "body-parser"] [@bs.val]
  external text_ : textOptions => t = "text";
  let text =
      (
        ~defaultCharset="utf-8",
        ~fileType="text/plain",
        ~inflate=true,
        ~limit: option(ByteLimit.t)=?,
        (),
      ) =>
    text_({
      "defaultCharset": defaultCharset,
      "type": fileType,
      "limit": ByteLimit.toBytes(limit),
      "inflate": inflate,
    });
  let urlencoded =
      (~extended=false, ~inflate=true, ~limit=?, ~parameterLimit=?, ()) =>
    urlencoded_({
      "inflate": inflate,
      "extended": extended,
      "parameterLimit": parameterLimit |> Js.Nullable.fromOption,
      "limit": ByteLimit.toBytes(limit),
    });
  [@bs.module "body-parser"] [@bs.val] external raw_ : rawOptions => t = "raw";
  let raw =
      (
        ~inflate=true,
        ~fileType="application/octet-stream",
        ~limit: option(ByteLimit.t)=?,
        (),
      ) =>
    raw_({
      "type": fileType,
      "limit": ByteLimit.toBytes(limit),
      "inflate": inflate,
    });
  module type S = {
    type f;
    type errorF;
    let from: f => t;
    /* Generate the common Middleware binding function for a given
     * type. This Functor is used for the Router and App classes. */
    let fromError: errorF => t;
  };
  module type ApplyMiddleware = {
    type f;
    let apply: (f, next, Request.t, Response.t) => unit;
    type errorF;
    let applyWithError: (errorF, next, Error.t, Request.t, Response.t) => unit;
  };
  module Make =
         (A: ApplyMiddleware)
         : (S with type f = A.f and type errorF = A.errorF) => {
    type f = A.f;
    external unsafeFrom : 'a => t = "%identity";
    let from = middleware => {
      let aux = (next, content, _) => next(content);
      unsafeFrom((req, res, next) =>
        A.apply(middleware, aux(next), req, res)
      );
    };
    type errorF = A.errorF;
    let fromError = middleware => {
      let aux = (next, content, _) => next(content);
      unsafeFrom((err, req, res, next) =>
        A.applyWithError(middleware, aux(next), err, req, res)
      );
    };
  };
  include
    Make(
      {
        type f = (next, Request.t, Response.t) => complete;
        type errorF = (next, Error.t, Request.t, Response.t) => complete;
        let apply = (f, next, req, res) =>
          (
            try (f(next, req, res)) {
            | e => next(Next.error(e), res)
            }
          )
          |> ignore;
        let applyWithError = (f, next, err, req, res) =>
          (
            try (f(next, err, req, res)) {
            | e => next(Next.error(e), res)
            }
          )
          |> ignore;
      },
    );
};

module PromiseMiddleware =
  Middleware.Make(
    {
      type f =
        (Middleware.next, Request.t, Response.t) => Js.Promise.t(complete);
      type errorF =
        (Middleware.next, Error.t, Request.t, Response.t) =>
        Js.Promise.t(complete);
      external castToErr : Js.Promise.error => Error.t = "%identity";
      let apply = (f, next, req, res) => {
        let promise: Js.Promise.t(complete) =
          try (f(next, req, res)) {
          | e => Js.Promise.resolve(next(Next.error(e), res))
          };
        promise
        |> Js.Promise.catch(err => {
             let err = castToErr(err);
             Js.Promise.resolve(next(Next.error(err), res));
           })
        |> ignore;
      };
      let applyWithError = (f, next, err, req, res) => {
        let promise: Js.Promise.t(complete) =
          try (f(next, err, req, res)) {
          | e => Js.Promise.resolve(next(Next.error(e), res))
          };
        promise
        |> Js.Promise.catch(err => {
             let err = castToErr(err);
             Js.Promise.resolve(next(Next.error(err), res));
           })
        |> ignore;
      };
    },
  );

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

module MakeBindFunctions = (T: {type t;}) : (Routable with type t = T.t) => {
  type t = T.t;
  [@bs.send] external use : (T.t, Middleware.t) => unit = "";
  [@bs.send]
  external useWithMany : (T.t, array(Middleware.t)) => unit = "use";
  [@bs.send]
  external useOnPath : (T.t, ~path: string, Middleware.t) => unit = "use";
  [@bs.send]
  external useOnManyPaths : (T.t, ~paths: array(string), Middleware.t) => unit = "use";
  [@bs.send]
  external useOnPathWithMany :
    (T.t, ~path: string, array(Middleware.t)) => unit =
    "use";
  [@bs.send] external get : (T.t, ~path: string, Middleware.t) => unit = "";
  [@bs.send] external getOnManyPaths : (T.t, ~paths: array(string), Middleware.t) => unit = "get";
  [@bs.send]
  external getWithMany : (T.t, ~path: string, array(Middleware.t)) => unit =
    "get";
  [@bs.send]
  external param : (T.t, ~name: string, Middleware.t) => unit = "param";
  [@bs.send] external post : (T.t, ~path: string, Middleware.t) => unit = "";
  [@bs.send] external postOnManyPaths : (T.t, ~paths: array(string), Middleware.t) => unit = "post";
  [@bs.send]
  external postWithMany : (T.t, ~path: string, array(Middleware.t)) => unit =
    "post";
  [@bs.send] external put : (T.t, ~path: string, Middleware.t) => unit = "";
  [@bs.send] external putOnManyPaths : (T.t, ~paths: array(string), Middleware.t) => unit = "put";
  [@bs.send]
  external putWithMany : (T.t, ~path: string, array(Middleware.t)) => unit =
    "put";
  [@bs.send] external patch : (T.t, ~path: string, Middleware.t) => unit = "";
  [@bs.send]
  external patchWithMany : (T.t, ~path: string, array(Middleware.t)) => unit =
    "patch";
  [@bs.send] external delete : (T.t, ~path: string, Middleware.t) => unit = "";
  [@bs.send]
  external deleteWithMany : (T.t, ~path: string, array(Middleware.t)) => unit =
    "delete";
};

module Router = {
  include
    MakeBindFunctions(
      {
        type t;
      },
    );
  type routerArgs = {
    .
    "caseSensitive": bool,
    "mergeParams": bool,
    "strict": bool,
  };
  [@bs.module "express"] [@bs.val] external make_ : routerArgs => t = "Router";
  let make = (~caseSensitive=false, ~mergeParams=false, ~strict=false, ()) =>
    make_({
      "caseSensitive": caseSensitive,
      "mergeParams": mergeParams,
      "strict": strict,
    });
  external asMiddleware : t => Middleware.t = "%identity";
};

let router = Router.make;

module HttpServer = {
  type t;
};

module App = {
  include
    MakeBindFunctions(
      {
        type t;
      },
    );
  let useRouter = (app, router) => Router.asMiddleware(router) |> use(app);
  let useRouterOnPath = (app, ~path, router) =>
    Router.asMiddleware(router) |> useOnPath(app, ~path);
  [@bs.module] external make : unit => t = "express";

  /*** [make ()] creates an instance of the App class. */
  external asMiddleware : t => Middleware.t = "%identity";

  /*** [asMiddleware app] casts an App instance to a Middleware type */
  [@bs.send]
  external listen_ :
    (t, int, [@bs.uncurry] (Js.Null_undefined.t(Js.Exn.t) => unit)) =>
    HttpServer.t =
    "listen";
  let listen = (app, ~port=3000, ~onListen=(_) => (), ()) =>
    listen_(app, port, onListen);
};

let express = App.make;


/*** [express ()] creates an instance of the App class.
     Alias for [App.make ()] */
module Static = {
  type options;
  type stat;
  let defaultOptions: unit => options =
    () => (Obj.magic(Js_obj.empty()): options);
  [@bs.set] external dotfiles : (options, string) => unit = "";
  [@bs.set] external etag : (options, bool) => unit = "";
  [@bs.set] external extensions : (options, array(string)) => unit = "";
  [@bs.set] external fallthrough : (options, bool) => unit = "";
  [@bs.set] external immutable : (options, bool) => unit = "";
  [@bs.set] external indexBool : (options, bool) => unit = "index";
  [@bs.set] external indexString : (options, string) => unit = "index";
  [@bs.set] external lastModified : (options, bool) => unit = "";
  [@bs.set] external maxAge : (options, int) => unit = "";
  [@bs.set] external redirect : (options, bool) => unit = "";
  [@bs.set] external setHeaders : (options, (Request.t, string, stat) => unit) => unit = "";

  type t;
  [@bs.module "express"] external make : (string, options) => t = "static";

  /*** [make directory] creates a static middleware for [directory] */
  external asMiddleware : t => Middleware.t = "%identity";
  /*** [asMiddleware static] casts [static] to a Middleware type */
};
