external make_no_arg : unit => Express.Middleware.t = "cookie-parser" 
  [@@bs.module];

external make_with_secret : string => Express.Middleware.t = "cookie-parser" 
  [@@bs.module];

let make ::secret=? () => {
  switch secret {
    | None => make_no_arg ();
    | Some secret => make_with_secret secret;
  };
};
