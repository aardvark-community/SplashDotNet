open System.Threading
open Aardvark.Base
open SplashDotNet

[<EntryPoint>]
let main argv = 
    //Aardvark.Init()
    let d = Splash.show @"C:\Users\Schorsch\Development\Brusher\src\Brusher\resources\splash.png"

    printfn "showed"
    Thread.Sleep(3000) 
    printfn "close"
    d.Dispose()
    printfn "closed"
    Thread.Sleep(3000) 
    printfn "yeah"
    //thread.Join()
    0
