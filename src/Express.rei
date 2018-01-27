type done_;

module Error: {type t; let message: t => option(string); let name: t => option(string);};

module Request: {
  type t;
  type params = Js.Dict.t(Js.Json.t);
  let params: t => params;
  let asJsonObject: t => Js.Dict.t(Js.Json.t);
  let baseUrl: t => string;
  let bodyJSON: t => option(Js.Json.t);
  let bodyRaw: t => option(Node_buffer.t);
  let bodyText: t => option(string);
  let bodyURLEncoded: t => option(Js.Dict.t(string));
  let cookies: t => option(Js.Dict.t(Js.Json.t));
  let signedCookies: t => option(Js.Dict.t(Js.Json.t));
  let hostname: t => string;
  let ip: t => string;
  let fresh: t => bool;
  let stale: t => bool;
  let methodRaw: t => string;
  type method =
    | Get
    | Post
    | Put
    | Delete
    | Head
    | Options
    | Trace
    | Connect;
  let method: t => method;
  let originalUrl: t => string;
  let path: t => string;
  type protocol =
    | Http
    | Https;
  let protocol: t => protocol;
  let secure: t => bool;
  let query: t => Js.Dict.t(Js.Json.t);
  let accepts: (t, array(string)) => option(string);
  let acceptsCharsets: (t, array(string)) => option(string);
  let get: (t, string) => option(string);
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
      | MultipleChoises
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
  let sendFile: (t, string, 'a) => done_;
  let sendString: (t, string) => done_;
  let sendJson: (t, Js.Json.t) => done_;
  let sendBuffer: (t, Buffer.t) => done_;
  let sendArray: (t, array('a)) => done_;
  let sendRawStatus: (t, int) => done_;
  let sendStatus: (t, StatusCode.t) => done_;
  let rawStatus: (t, int) => t;
  let status: (t, StatusCode.t) => t;
  let json: (t, Js.Json.t) => done_;
  let redirectCode: (t, int, string) => done_;
  let redirect: (t, string) => done_;
};

module Next: {
  type content;
  type t = Js.undefined(content) => done_;
  let middleware: Js.undefined(content);
  let route: Js.undefined(content);
  let error: Error.t => Js.undefined(content);
};

module Middleware: {
  type next = Next.t;
  type t;
  type f = (Request.t, Response.t, next) => done_;
  let from: f => t;
  type errorF = (Error.t, Request.t, Response.t, next) => done_;
  let fromError: errorF => t;
};

module MakeBindFunctions:
  (T: {type t;}) =>
  {
    let use: (T.t, Middleware.t) => unit;
    let useWithMany: (T.t, array(Middleware.t)) => unit;
    let useOnPath: (T.t, ~path: string, Middleware.t) => unit;
    let useOnPathWithMany: (T.t, ~path: string, array(Middleware.t)) => unit;
    let get: (T.t, ~path: string, Middleware.t) => unit;
    let getWithMany: (T.t, ~path: string, array(Middleware.t)) => unit;
    let post: (T.t, ~path: string, Middleware.t) => unit;
    let postWithMany: (T.t, ~path: string, array(Middleware.t)) => unit;
    let put: (T.t, ~path: string, Middleware.t) => unit;
    let putWithMany: (T.t, ~path: string, array(Middleware.t)) => unit;
    let patch: (T.t, ~path: string, Middleware.t) => unit;
    let patchWithMany: (T.t, ~path: string, array(Middleware.t)) => unit;
    let delete: (T.t, ~path: string, Middleware.t) => unit;
    let deleteWithMany: (T.t, ~path: string, array(Middleware.t)) => unit;
  };

module App: {
  type t;
  let use: (t, Middleware.t) => unit;
  let useWithMany: (t, array(Middleware.t)) => unit;
  let useOnPath: (t, ~path: string, Middleware.t) => unit;
  let useOnPathWithMany: (t, ~path: string, array(Middleware.t)) => unit;
  let get: (t, ~path: string, Middleware.t) => unit;
  let getWithMany: (t, ~path: string, array(Middleware.t)) => unit;
  let post: (t, ~path: string, Middleware.t) => unit;
  let postWithMany: (t, ~path: string, array(Middleware.t)) => unit;
  let put: (t, ~path: string, Middleware.t) => unit;
  let putWithMany: (t, ~path: string, array(Middleware.t)) => unit;
  let patch: (t, ~path: string, Middleware.t) => unit;
  let patchWithMany: (t, ~path: string, array(Middleware.t)) => unit;
  let delete: (t, ~path: string, Middleware.t) => unit;
  let deleteWithMany: (t, ~path: string, array(Middleware.t)) => unit;
  let make: unit => t;
  let asMiddleware: t => Middleware.t;
  let listen: (t, ~port: int=?, ~onListen: Js.Null_undefined.t(Js.Exn.t) => unit=?, unit) => unit;
};

let express: unit => App.t;

module Static: {
  type options;
  let defaultOptions: unit => options;
  let dotfiles: (options, string) => unit;
  let etag: (options, Js.boolean) => unit;
  type t;
  let make: (string, options) => t;
  let asMiddleware: t => Middleware.t;
};