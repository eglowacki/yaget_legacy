

public partial class vfs
{

    /// Return Type: void
    ///fileName: char*
    [System.Runtime.InteropServices.DllImportAttribute("Editor.dll", EntryPoint = "eg_vfs_FileChanged")]
    public static extern void fileChanged([System.Runtime.InteropServices.InAttribute()] [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.LPStr)] string fileName);
}

