open Express;

let app = express ();

App.useOnPath app path::"/" @@ Middleware.from (fun _ _ next => {
  Js.log "Request received"; 
     /* This will be printed for every request */
  next Js.undefined [@bs]
     /* call the next middleware in the processing pipeline */
});

App.useN app [|
  Middleware.from (fun _ _ next => {
    Js.log "trace #1";
    next Next.undefined [@bs];
  }),
  Middleware.from (fun _ _ next => {
    Js.log "trace #2";
    next Next.undefined [@bs];
  })
|]; 

App.get app path::"/" @@ Middleware.from (fun _ res _ => { 
  let json = Js_dict.empty ();
  Js_dict.set json "hello" (Js_json.string "World"); 
    /* This only works with dev version of BuckleScript */
  Response.sendJson res (Js_json.object_ json);
});

App.useOnPathN app path::"/static" @@ [| 
  Middleware.from (fun _ _ next => {
    Js.log "trace #3";
    next Next.undefined [@bs]; 
  }), 
  {
    let options = Static.defaultOptions (); 
    Static.make "static" options |> Static.asMiddleware 
  }, 
|]; 

App.listen app port::3000 ;
