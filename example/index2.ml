open Express2 

(* This is the exact same example from the Express documentation 
 * on how to create a Router *) 
module Birds = struct 

  let router = Router.make () 

  let () =
    Router.use router @@ Middleware.ofF (fun _ _ next -> 

      let date = 
        Js.Date.now () 
        |> Js.Date.fromFloat
        |> Js.Date.toString
      in 
      Js.log ("Time: " ^ date);
      next Js.undefined [@bs]
    ); 

    Router.get router "/" @@ Middleware.ofF (fun _ res _ -> 
      Response.sendString res "Birds Home Page"
    ); 

    Router.get router "/about" @@ Middleware.ofF (fun _ res _ -> 
      Response.sendString res "About birds"
    )
end 

let () = 
  let app = App.express () in 

  (* Show how to add a Middleware made out of a Router class *)
  App.useOnPath app "/birds" (Birds.router |> Router.toMiddleware); 

  (* -- *)

  (* Show how to add a Middleware from a function *)
  App.use app @@ Middleware.ofF (fun _ _ next -> 
    Js.log "Request received";
    next Js.undefined [@bs]
  ); 

  (* -- *)

  (* Show how to add an Error Handling Middleware *)
  App.use app @@ Middleware.ofErrorF (fun err _ res _ -> 
    Js.log err; 
    Response.sendString res "error"
  ); 
     
  (* -- *)

  (* Show how to add multiple chained Middleware made of functions
   * for a given method and path *)

  let fromMiddlewares = [|

    Middleware.ofF (fun req _ next-> 
      Js.log @@ Request.params req; 
      print_endline "Just got who";
      next Js.undefined [@bs];
    );

    Middleware.ofF (fun _ res _ -> 
      Response.sendString res "Hello World"; 
    ); 

  |] in 
  
  App.getN app "/from/:who" fromMiddlewares;
  
  App.get app "/skip" @@ Middleware.ofF (fun _ _ next -> 
    next (Js.Undefined.return "router") [@bs]
  );

  (* -- *)

  (* Show how to add a static middleware *)

  App.useOnPath app "/static" (
    let options = Static.make_options () in 
    Static.etag options Js.true_; 
    Static.static "static-html" options |> Static.toMiddleware
  );  
  
  (* -- *)

  (* Show how to add a Third Party middleware *)

  App.use app (BodyParserJSON.make () |> BodyParserJSON.toMiddleware);

  App.listen app 8000;

  ()
