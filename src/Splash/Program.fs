open System.Threading
open Aardvark.Base
open SplashDotNet

[<EntryPoint>]
let main argv = 
    //Aardvark.Init()
    let d = Splash.show @"C:\Users\Schorsch\Desktop\test.jpg"

    printfn "showed"
    Thread.Sleep(3000) 
    printfn "close"
    d.Dispose()
    printfn "closed"
    Thread.Sleep(3000) 
    //thread.Join()
    0
