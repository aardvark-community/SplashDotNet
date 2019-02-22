namespace SplashDotNet

open System
open System.Runtime.InteropServices
open System.Runtime.CompilerServices
open System.Threading
open Aardvark.Base
open System.IO

module private SplashNative =
    open System.Security

    [<Literal>]
    let lib = "Splash.Native.dll"

    [<DllImport(lib); SuppressUnmanagedCodeSecurity>]
    extern uint32 ShowSplash(int w, int h, int bits, byte[] data)
    
    [<DllImport(lib, EntryPoint = "ShowSplash"); SuppressUnmanagedCodeSecurity>]
    extern uint32 ShowSplash_(int w, int h, int bits, void* data)
    
    [<DllImport(lib); SuppressUnmanagedCodeSecurity>]
    extern void CloseSplash(uint32 ptr);


type Splash =
    static member show (image : PixImage) =
        match Environment.OSVersion.Platform with
        | PlatformID.Unix | PlatformID.MacOSX ->  
            { new IDisposable with
                member x.Dispose() = ()
            }
        | _  ->
            let image = image.ToPixImage<byte>(Col.Format.BGRA)
            let p = SplashNative.ShowSplash(image.Size.X, image.Size.Y, 32, image.Volume.Data)
            { new IDisposable with
                member x.Dispose() = SplashNative.CloseSplash(p)
            }

    static member show (file : string) =
        let f = Path.Combine(Environment.CurrentDirectory, "DevIL.dll")
        if not (File.Exists f) then
            printfn "unpack"
            Aardvark.UnpackNativeDependencies(typeof<DevILSharp.AttributeBits>.Assembly)
        let img = PixImage.Create file
        Splash.show img
        
