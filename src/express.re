/*
  There are a few types to update in this file.
  We're generic in 'a in most types, but really
  should specify which JS type should be used.

  An example of how to handle JS fns that take
  more than one type, where some types are
  containers and some primitives is the the
  Response.send binding below.

  sendString | sendJson | sendBuffer | sendArray
  are all bindings to the .send method based on the
  input type

  tracked in https://github.com/BuckleTypes/Rexpress/issues/6
 */
module Next = {
  type t;
};

module Request = {
  type t;
};

module Response = {
  type t;
  external sendFile : t => string => 'a => unit = "" [@@bs.send];
  external sendString : t => string => unit = "send" [@@bs.send];
  external sendJson : t => Js.Json.t => unit = "send" [@@bs.send];
  external sendBuffer : t => Buffer.t => unit = "send" [@@bs.send];
  external sendArray : t => array 'a => unit = "send" [@@bs.send];
  external json : t => Js.Json.t => unit = "" [@@bs.send] [@@ocaml.deprecated "Use sendJson instead`"];
};

module Express = {
  type t;
  type middlewareT = Request.t => Response.t => Next.t => unit;
  external express : unit => t = "" [@@bs.module];
  external use : t => middlewareT => unit = "" [@@bs.send];
  external static : path::string => middlewareT = "" [@@bs.module "express"];
  external get : t => string => ('a => Response.t => unit) => unit = "" [@@bs.send];
  external listen : t => port::int => unit = "" [@@bs.send];
};
