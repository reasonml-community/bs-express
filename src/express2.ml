type done_
(** abstract type which ensure middleware function must either
    call the [next] function or one of the [send] function on the 
    response object. 

    This should be a great argument for OCaml, the type system 
    prevents silly error which in this case would make the server hang *)

(** TODO : maybe this should be a more common module like bs-error *)
module Error = struct 

  type t 

  external message : t -> string option = "" 
    [@@bs.send] [@@bs.return null_undefined_to_opt]
  
  external name : t -> string option = "" 
    [@@bs.send] [@@bs.return null_undefined_to_opt]

end 

module Request = struct 

  type t 

  type params = Js_json.t Js_dict.t

  external params : t -> params = "" [@@bs.get]
end 

module Response = struct 

  type t 

  external sendString : t -> string -> done_ = "send" [@@bs.send] 
end

module Middleware = struct 

  type t = string Js.undefined -> done_ [@bs]

  type t 
    (** Middleware abstract type which unified the various way an 
        Express middleware can be constructed:
        {ul
        {- from a {b function} using the [ofFunction]}  
        {- from a Router class}
        {- from an App class}
        {- from the third party middleware modules}   
        }
      
        For each of the class which implements the middleware interface 
        is JavaScript, one can use the "%identity" function to cast
        it to this type [t] *)

  type f = Request.t -> Response.t -> next -> done_

  external ofF : f -> t = "%identity" 
  (** Create a Middleware from a function *)

  type errorF = Error.t -> Request.t -> Response.t -> next -> done_ 

  external ofErrorF : errorF -> t = "%identity" 
  (** Create a Middleware from an error function *)

end 

(** Generate the common Middleware binding function for a given 
    type. This Functor is used for the Router and App classes. *)
module MakeBindFunctions(T: sig type t end) = struct 

  external use : T.t -> Middleware.t -> unit = "" [@@bs.send]
  
  external useOnPath : T.t -> string -> Middleware.t -> unit = "use" [@@bs.send]

  external get :  
    T.t -> 
    string -> 
    Middleware.t -> 
    unit = "" [@@bs.send] 

  external getN :  
    T.t -> 
    string -> 
    Middleware.t array -> 
    unit = "get" [@@bs.send] 

  external post :  
    T.t -> 
    string -> 
    Middleware.t -> 
    unit = "" [@@bs.send] 

  external postN :  
    T.t -> 
    string -> 
    Middleware.t array -> 
    unit = "post" [@@bs.send] 

  (* Add put/delete/all ... *)
end 

module App = struct 
  type app

  include MakeBindFunctions(struct type t = app end)
  
  type t = app

  external express : unit -> t = "" [@@bs.module] 

  external toMiddleware : t -> Middleware.t = "%identity"
  (** [toMiddleware app] casts an App  to a Middleware type *) 

  external listen : t -> int -> unit = "" [@@bs.send]

end 

module Router = struct 
  
  type router 

  include MakeBindFunctions(struct type t = router end)

  type t = router
  
  external make : unit -> t = "Router" [@@bs.module "express"] 

  external toMiddleware : t -> Middleware.t = "%identity" 
  (** [toMiddleware router] casts a Router to a Middleware type *)

end 

module Static = struct 

  type options 

  let make_options : unit -> options = fun () ->
    (Obj.magic (Js_obj.empty ()) : options) 

  external dotfiles : options -> string -> unit = "" [@@bs.set]

  external etag : options -> Js.boolean -> unit = "" [@@bs.set]
  
  (* ... add all the other options *) 
  
  type t 
 
  external static : string -> options -> t = "" [@@bs.module "express"] 

  external toMiddleware : t -> Middleware.t = "%identity"
  (** Cast a Static class to a Middleware *)

end 

(** Example of a Third Party Middleware binding.  
 *
 *  !!! This should really be in its own module !!!  *)

module BodyParserJSON = struct 

  type t (* JSON Middleware *)

  external make : unit -> t = "json" [@@bs.module "body-parser"]

  external toMiddleware : t -> Middleware.t = "%identity"

  (* One could skip the identity function and have the json function
   * returns directly a Middleware.t *) 

end 

