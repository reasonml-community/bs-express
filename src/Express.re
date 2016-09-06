let module Express = {
  type expressT;
  external express : unit => expressT = "" [@@bs.module];
  external use : expressT => string => unit = "" [@@bs.send];
  external static : string => string = "" [@@bs.module "express"];
  type responseT;
  type requestT;
  external sendFile : responseT => string => 'a => unit = "" [@@bs.send];
  external json: responseT => 'a => unit = "" [@@bs.send];
  external get : expressT => string => ('a => responseT => unit [@bs]) => unit = "" [@@bs.send]
};
