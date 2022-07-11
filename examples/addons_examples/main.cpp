#include <imgui.h>
#ifndef NO_IMGUIDATETIME
#include <time.h>   // very common plain c header file used only by datetime widget
#endif //NO_IMGUIDATETIME


// Helper stuff we'll use later ----------------------------------------------------
ImTextureID myImageTextureId2 = 0;
static ImVec2 gMainMenuBarSize(0,0);
static void ShowExampleAppMainMenuBar() {
    if (ImGui::BeginMainMenuBar())  {
        if (ImGui::BeginMenu("Edit"))   {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        gMainMenuBarSize = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }
}
#ifndef NO_IMGUILISTVIEW
inline void MyTestListView() {
    ImGui::Spacing();
    static ImGui::ListView lv;
    if (lv.headers.size()==0) {
        lv.headers.push_back(ImGui::ListViewHeader("Icon",NULL,ImGui::ListView::HT_ICON,-1,20));
        lv.headers.push_back(ImGui::ListViewHeader("Index",NULL,ImGui::ListView::HT_INT,-1,35));
        lv.headers.push_back(ImGui::ListViewHeader("Path",NULL,ImGui::ListView::HT_STRING,-1,80,"","",true,ImGui::ListViewHeaderEditing(true,1024)));
        lv.headers.push_back(ImGui::ListViewHeader("Offset",NULL,ImGui::ListView::HT_INT,-1,40,"","",true));
        lv.headers.push_back(ImGui::ListViewHeader("Bytes","The number of bytes",ImGui::ListView::HT_UNSIGNED,-1,40));
        lv.headers.push_back(ImGui::ListViewHeader("Valid","A boolean flag",ImGui::ListView::HT_BOOL,-1,70,"Flag: ","!",true,ImGui::ListViewHeaderEditing(true)));
        lv.headers.push_back(ImGui::ListViewHeader("Length","A float[3] array",ImGui::ListViewHeaderType(ImGui::ListView::HT_FLOAT,3),2,105,""," mt",ImGui::ListViewHeaderSorting(true,1),ImGui::ListViewHeaderEditing(true,3,-180.0,180.0))); // Note that here we use 2 decimals (precision), but 3 when editing; we use an explicit call to "ListViewHeaderType",specifying that the HT_FLOAT is composed by three elements; we have used an explicit call to "ListViewHeaderSorting" specifying that the items must be sorted based on the second float.
        lv.headers.push_back(ImGui::ListViewHeader("Color",NULL,ImGui::ListView::HT_COLOR,-1,85,"","",true,ImGui::ListViewHeaderEditing(true))); // precision = -1 -> Hex notation; precision > 1 -> float notation; other = undefined behaviour. To display alpha we must use "ListViewHeaderType" explicitely like in the line above, specifying 4.

        // Warning: old compilers don't like defining classes inside function scopes
        class MyListViewItem : public ImGui::ListView::ItemBase {
        public:
            // Support static method for enum1 (the signature is the same used by ImGui::Combo(...))
            static bool GetTextFromEnum1(void* ,int value,const char** pTxt) {
                if (!pTxt) return false;
                static const char* values[] = {"APPLE","LEMON","ORANGE"};
                static int numValues = (int)(sizeof(values)/sizeof(values[0]));
                if (value>=0 && value<numValues) *pTxt = values[value];
                else *pTxt = "UNKNOWN";
                return true;
            }

            // Fields and their pointers (MANDATORY!)
            ImGui::ListViewIconData icon;
            int index;
            char path[1024];            // Note that if this column is editable, we must specify: ImGui::ListViewHeaderEditing(true,1024); in the ImGui::ListViewHeader::ctr().
            int offset;
            unsigned bytes;
            bool valid;
            float length[3];
            ImVec4 color;
            int enum1;      // Note that it's an enum!
            const void* getDataPtr(size_t column) const {
                switch (column) {
                case 0: return (const void*) &icon;
                case 1: return (const void*) &index;
                case 2: return (const void*) path;
                case 3: return (const void*) &offset;
                case 4: return (const void*) &bytes;
                case 5: return (const void*) &valid;
                case 6: return (const void*) &length[0];
                case 7: return (const void*) &color;
                case 8: return (const void*) &enum1;
                }
                return NULL;
                // Please note that we can easily try to speed up this method by adding a new field like:
                // const void* fieldPointers[number of fields];    // and assigning them in our ctr
                // Then here we can just use:
                // IM_ASSERT(column<number of fields);
                // return fieldPointers[column];
            }

            // (Optional) ctr for setting values faster later
            MyListViewItem(const ImGui::ListViewIconData& _icon,int _index,const char* _path,int _offset,unsigned _bytes,bool _valid,const ImVec4& _length,const ImVec4& _color,int _enum1)
                : icon(_icon),index(_index),offset(_offset),bytes(_bytes),valid(_valid),color(_color),enum1(_enum1) {
                IM_ASSERT(_path && strlen(_path)<1024);
                strcpy(path,_path);
                length[0] = _length.x;length[1] = _length.y;length[2] = _length.z;  // Note that we have used "ImVec4" for _length, just because ImVec3 does not exist...
            }
            virtual ~MyListViewItem() {}

        };

        // for enums we must use the ctr that takes an ImGui::ListViewHeaderType, so we can pass the additional params to bind the enum:
        lv.headers.push_back(ImGui::ListViewHeader("Enum1","An editable enumeration",ImGui::ListViewHeaderType(ImGui::ListView::HT_ENUM,3,&MyListViewItem::GetTextFromEnum1),-1,-1,"","",true,ImGui::ListViewHeaderEditing(true)));

        // Just a test: 10000 items
        lv.items.resize(10000);
        MyListViewItem* item;ImGui::ListViewIconData ti;
        for (int i=0,isz=(int)lv.items.size();i<isz;i++) {

            ti.user_texture_id = reinterpret_cast<void*> (myImageTextureId2);
            float y = (float)(i/3)/3.f,x=(float)(i%3)/3.f;
            ti.uv0.x=x;ti.uv0.y=y;
            ti.uv1.x=x+1.f/3.f;ti.uv1.y=y+1.f/3.f;
            //ti.tint_col.z=ti.tint_col.x=0;          // tint color = image color is multiplied by this color
            ti.bg_col.x=ti.bg_col.y=ti.bg_col.z=1;ti.bg_col.w=1;  // bg color (used in transparent pixels of the image)

            item = (MyListViewItem*) ImGui::MemAlloc(sizeof(MyListViewItem));                       // MANDATORY (ImGuiListView::~ImGuiListView() will delete these with ImGui::MemFree(...))
            IMIMPL_PLACEMENT_NEW(item) MyListViewItem(
                        ti,
                        i,
                        "My '  ' Dummy Path",
                        i*3,
                        (unsigned)i*4,(i%3==0)?true:false,
                        ImVec4((float)(i*30)/2.7345672,(float)(i%30)/2.7345672,(float)(i*5)/1.34,1.f),  // ImVec3 does not exist... so we use an ImVec4 to initialize a float[3]
                        ImVec4((float)i/(float)(isz-1),0.8f,1.0f-(float)i/(float)(isz-1),1.0f),         // HT_COLOR
                        i%3
            );    // MANDATORY even with blank ctrs. Reason: ImVector does not call ctrs/dctrs on items.
            item->path[4]=(char) (33+(i%64));   //just to test sorting on strings
            item->path[5]=(char) (33+(i/127));  //just to test sorting on strings
            lv.items[i] = item;
        }

        //lv.setColorEditingMode(ImGuiColorEditMode_HSV);   // Optional, but it's window-specific: it affects everything in this window AFAIK
    }

    // 2 lines just to have some feedback
    if (ImGui::Button("Scroll to selected row")) {lv.scrollToSelectedRow();}    ImGui::SameLine();
    ImGui::Text("selectedRow:%d selectedColumn:%d isInEditingMode:%s",lv.getSelectedRow(),lv.getSelectedColumn(),lv.isInEditingMode() ? "true" : "false");

    /*
    static ImVector<int> optionalColumnReorder;
    if (optionalColumnReorder.size()==0) {
        const int numColumns = lv.headers.size();
        optionalColumnReorder.resize(numColumns);
        for (int i=0;i<numColumns;i++) optionalColumnReorder[i] = numColumns-i-1;
    }
    */

    static int maxListViewHeight=200;                             // optional: by default is -1 = as height as needed
    ImGui::SliderInt("ListView Height (-1=full)",&maxListViewHeight,-1,500);// Just Testing "maxListViewHeight" here:

    lv.render((float)maxListViewHeight);//(float)maxListViewHeight,&optionalColumnReorder,-1);   // This method returns true when the selectedRow is changed by the user (however when selectedRow gets changed because of sorting it still returns false, because the pointed row-item does not change)

}
#endif //NO_IMGUILISTVIEW
#ifndef NO_IMGUIVARIOUSCONTROLS
// Completely optional Callbacks for ImGui::TreeView
static bool MyTreeViewNodeDrawIconCallback(ImGui::TreeViewNode* n, ImGui::TreeView& /*tv*/, void* /*userPtr*/)    {
    // We use the node "userId" (an int) to store our icon index
    int i = n->getUserId();
    if (i>0 && i<10)   {
        --i;   // in [0,8]
        const ImVec2 uv0((float)(i%3)/3.f,(float)(i/3)/3.f);
        const ImVec2 uv1(uv0.x+1.f/3.f,uv0.y+1.f/3.f);
        ImVec2 size;size.x=size.y=ImGui::GetTextLineHeight();
        ImGui::Image(myImageTextureId2,size,uv0,uv1);   // However when checkboxes are present this stays on the top (and no idea on how to fix it...)
        const bool hovered = ImGui::IsItemHovered();
        ImGui::SameLine();  // Mandatory
        return hovered;
    }
    else {
        // Maybe we should skip ImGui::GetTextLineHeight() pixels here...
    }
    return false;
    // Of course it's possible (and faster) to add icons from a ttf file...
}
static void MyTreeViewNodeCreationDelationCallback(ImGui::TreeViewNode* n, ImGui::TreeView&,bool delation,void* ) {
    IM_ASSERT(delation ? (n->userPtr!=NULL) : (n->userPtr==NULL));
    if (delation) ImGui::MemFree(n->userPtr);
    else n->userPtr = ImGui::MemAlloc(25);
    fprintf(stderr,"%s: [%s]\n",n->getDisplayName(),delation?"Deletion":"Creation");
}
#endif //NO_IMGUIVARIOUSCONTROLS
#ifdef YES_IMGUISQLITE3
static void PerformCppSQLiteTest(ImGuiTextBuffer &rv, int nRowsToCreate=50000);
#endif //YES_IMGUISQLITE3

// These are only needed if you need to modify them at runtime (almost never).
// Otherwise you can set them directly in "main" (see at the botton of this file).
extern bool gImGuiDynamicFPSInsideImGui;
extern float gImGuiInverseFPSClampInsideImGui;
extern float gImGuiInverseFPSClampOutsideImGui;
//------------------------------------------------------------------------------------

static const char* styleFileName = "./myimgui.style";
static const char* styleFileNamePersistent = "/persistent_folder/myimgui.style";   // Needed by Emscripten only


void InitGL()	// Mandatory
{

    if (!myImageTextureId2) myImageTextureId2 = ImImpl_LoadTexture("./myNumbersTexture.png");

//  Optional: loads a style
#   if (!defined(NO_IMGUISTYLESERIALIZER) && !defined(NO_IMGUISTYLESERIALIZER_LOAD_STYLE))
    const char* pStyleFileName = styleFileName;
#   if (defined(YES_IMGUIEMSCRIPTENPERSISTENTFOLDER) && !defined(NO_IMGUIHELPER) && !defined(NO_IMGUIHELPER_SERIALIZATION) && !defined(NO_IMGUIHELPER_SERIALIZATION_LOAD))
    //ImGui::EmscriptenFileSystemHelper::Init();
    //while (!ImGui::EmscriptenFileSystemHelper::IsInSync()) {WaitFor(1500);}   // No way this ends...
    //if (ImGuiHelper::FileExists(styleFileNamePersistent)) pStyleFileName = styleFileNamePersistent; // Never...
    //else {printf("\"%s\" does not exist yet\n",styleFileNamePersistent);fflush(stdout);}
#   endif //YES_IMGUIEMSCRIPTENPERSISTENTFOLDER & C
    if (!ImGui::LoadStyle(pStyleFileName,ImGui::GetStyle()))   {
        printf("Warning: \"%s\" not present.\n",pStyleFileName);fflush(stdout);
        ImGui::ResetStyle(ImGuiStyle_Gray);
    }
#   endif //NO_IMGUISTYLESERIALIZER
// We might just choose one predefined style:
//ImGui::ResetStyle(ImGuiStyle_Gray,ImGui::GetStyle());

// This is something that does not work properly with all the addons:
//ImGui::GetIO().NavFlags |= ImGuiNavFlags_EnableKeyboard;
}
void ResizeGL(int /*w*/,int /*h*/)	// Mandatory
{

}
void DestroyGL()    // Mandatory
{
    if (myImageTextureId2) {ImImpl_FreeTexture(myImageTextureId2);}

}
void DrawGL()	// Mandatory
{

    const ImVec4 defaultClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    static ImVec4 clearColor = defaultClearColor;
    ImImpl_ClearColorBuffer(clearColor);    // Warning: it does not clear depth buffer

    const ImGuiTreeNodeFlags collapsingHeaderFlags = ImGuiTreeNodeFlags_CollapsingHeader & (~ImGuiTreeNodeFlags_NoTreePushOnOpen);

    // Pause/Resume ImGui and process input as usual
    if (!ImGui::GetIO().WantCaptureKeyboard)    {
        if (ImGui::IsKeyPressed(ImGuiKey_H,false)
                || ImGui::IsKeyPressed(ImGuiKey_F1,false))
            gImGuiPaused = !gImGuiPaused;       // TODO: fix minor visual bug (if possible) happening at the first "restoring ImGui" frame (comment out the line "if (gImGuiPaused) return;" to see it)
    }

    // More detailed example on how to overuse ImGui to process input. This works even when ImGui is paused:
    if (gImGuiWereOutsideImGui) // true when "gImGuiPaused" or simply when we're outside ImGui Windows with both mouse and cursor.
    {
        ImGuiIO& io = ImGui::GetIO();
        static unsigned myStrangeCounter=0;

        if (!io.WantCaptureKeyboard && !io.WantCaptureMouse && !io.WantTextInput )   // always "true" if we "leave" "if (gImGuiWereOutsideImGui)" AFAIR
        {

            // (Key and Mouse) Input events fall into two main categories:
            // 1) "stateful" events (= pressed or released events) are easier to process. e.g.:

            if (ImGui::IsKeyPressed(ImGuiKey_H, false))
            {printf("'H' key pressed outside Imgui (%u)\n",myStrangeCounter++);fflush(stdout);}
            // Tips for processing other "stateful" events (e.g. key/mouse pressed/released):
            // a) for ImGui "known special chars", we can use something like:
            //      if (ImGui::IsKeyPressed(ImGuiKey_Escape,false))  {...}
            // b) for mouse Pressed/Released events:
            //      if (io.MousePressed[...]) {...}

            // 2) "immediate" (or "continuous") events (= down events)
            // When taking actions based on continuous events (e.g. "down events"), IMHO it's better to sync
            // them to ensure the same behavior at differernt FPS.
            // "inputProcessingInterval" should be a fixed amount >= our inverse frame rate.
            // However, since we will later allow the user to modify the frame rate, we can't leave it constant here.
            const float inputProcessingInterval = gImGuiInverseFPSClampOutsideImGui<=0 ? 0.03 : gImGuiInverseFPSClampOutsideImGui*2.f;  // Input processing frequency (better leave it constant)
            static float timer = ImGui::GetTime();
            float delta = ImGui::GetTime() - timer;
            if (delta<inputProcessingInterval) {
                if (delta<0) timer = ImGui::GetTime();  // protects from overflow ? (probably just an intention)
            }
            else {
                timer+=delta;
                //-------------------------------------------------------------------
                if (ImGui::IsKeyDown(ImGuiKey_RightArrow)) {printf("Right arrow pressed outside Imgui (%u)\n",myStrangeCounter++);fflush(stdout);}
                //if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {printf("Middle Mouse Button pressed outside Imgui (%u)\n",myStrangeCounter++);fflush(stdout);}
                //-------------------------------------------------------------------
            }
        }

    }

    if (gImGuiPaused) return; // exit early (even if ocassional ImGui calls should be allowed, as "immediate mode GUI" is made to mix GUI calls with normal code)


    ShowExampleAppMainMenuBar();    // This is plain ImGui

    static bool show_test_window = true;
    static bool show_another_window = false;
    static bool show_node_graph_editor_window = false;
    static bool show_dock_window = false;
    static bool show_tab_windows = false;
    static bool show_performance = false;
    static bool show_mine_game = false;
    static bool show_sudoku_game = false;
    static bool show_fifteen_game = false;
    static bool show_image_editor = false;

    // 1. Show a simple window
    static bool open = true;static float bg_alpha = -1.f;
    //if (ImGui::Begin("ImGui Addons", &open, ImVec2(450,300),bg_alpha,0))   // Old API
    ImGui::SetNextWindowSize(ImVec2(450,300), ImGuiCond_FirstUseEver);
    if (bg_alpha>=0.f) ImGui::SetNextWindowBgAlpha(bg_alpha);
    if (ImGui::Begin("ImGui Addons",&open,0))
    {

        if (ImGui::TreeNodeEx("Pause/Resume ImGui and process input as usual",collapsingHeaderFlags)) {
        //ImGui::Text("\n");ImGui::Separator();ImGui::Text("Pause/Resume ImGui and process input as usual");ImGui::Separator();
        ImGui::Text("Press F1 (or 'h') to turn ImGui on and off.");
        ImVec4 halfTextColor = ImGui::GetStyle().Colors[ImGuiCol_Text];halfTextColor.w*=0.5f;
        ImGui::TextColored(halfTextColor,"(Please read the code for further tips about input processing).");
        //ImGui::Spacing();
        ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Other Windows",collapsingHeaderFlags|ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginGroup();
#       if (!defined(NO_IMGUISTYLESERIALIZER) && !defined(NO_IMGUISTYLESERIALIZER_SAVE_STYLE))
        show_test_window ^= ImGui::Button("Test Window");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","The default ImGui\n\"Test Window\"\n(useful for making\ncustom styles)");
#       endif //NO_IMGUISTYLESERIALIZER
#       ifndef NO_IMGUITOOLBAR
        show_another_window ^= ImGui::Button("Toolbar Test");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","An example of the imguitoolbar addon\nused inside a window");
#       endif //NO_IMGUITOOLBAR
#       ifndef NO_IMGUINODEGRAPHEDITOR
        show_node_graph_editor_window ^= ImGui::Button("ImGui Node Graph Editor");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","From the imguinodegrapheditor addon");
#       endif //NO_IMGUINODEGRAPHEDITOR
#       ifndef NO_IMGUIDOCK
        show_dock_window ^= ImGui::Button("ImGui Dock");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","An example of imguidock\n(LumixEngine's Docking System)");
#       endif //NO_IMGUIDOCK
#       ifndef NO_IMGUITABWINDOW
        show_tab_windows ^= ImGui::Button("ImGui Tab Window");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","An (incomplete) example of using\nthe imguitabwindow addon.\nMore on this in the second demo (main2.cpp)");
#       endif //NO_IMGUITABWINDOW
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
        show_performance ^= ImGui::Button("Show performance");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Performance Window.\nShows the frame rate and the\n number of texture switches per frame");
#       ifdef YES_IMGUIMINIGAMES
#           ifndef NO_IMGUIMINIGAMES_MINE
            show_mine_game ^= ImGui::Button("ImGui Mine Game");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","An example from the\nimguiminigames yes_addon");
#           endif //NO_IMGUIMINIGAMES_MINE
#           ifndef NO_IMGUIMINIGAMES_SUDOKU
            show_sudoku_game ^= ImGui::Button("ImGui Sudoku Game");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Another example from the\nimguiminigames yes_addon");
#           endif //NO_IMGUIMINIGAMES_SUDOKU
#           ifndef NO_IMGUIMINIGAMES_FIFTEEN
            show_fifteen_game ^= ImGui::Button("ImGui Fifteen Game");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Another example from the\nimguiminigames yes_addon");
#           endif //NO_IMGUIMINIGAMES_FIFTEEN
#       endif //YES_IMGUIMINIGAMES
#       ifdef YES_IMGUIIMAGEEDITOR
            show_image_editor ^= ImGui::Button("ImGui Image Editor");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","An example window using the\nimguiimageeditor yes_addon");
#       endif //YES_IMGUIIMAGEEDITOR
        ImGui::EndGroup();
        ImGui::TreePop();
        }

        // Calculate and show framerate
        if (ImGui::TreeNodeEx("Frame rate options",collapsingHeaderFlags)) {
        ImGui::TextWrapped("%s","It might be necessary to move the mouse \"outside\" and \"inside\" ImGui for these options to update properly.");
        ImGui::Separator();
        ImGui::Text("Frame rate %.1f FPS (average %.3f ms/frame)",ImGui::GetIO().Framerate,1000.0f / ImGui::GetIO().Framerate);
        bool clampFPSOutsideImGui = gImGuiInverseFPSClampOutsideImGui > 0;
        ImGui::Checkbox("Clamp FPS when \"outside\" ImGui.",&clampFPSOutsideImGui);
        if (clampFPSOutsideImGui)    {
            if (gImGuiInverseFPSClampOutsideImGui<=0) gImGuiInverseFPSClampOutsideImGui = 1.f/60.f;
            float FPS = 1.f/gImGuiInverseFPSClampOutsideImGui;
            if (ImGui::SliderFloat("FPS when \"outside\" ImGui",&FPS,5.f,60.f,"%.2f")) gImGuiInverseFPSClampOutsideImGui = 1.f/FPS;
        }
        else gImGuiInverseFPSClampOutsideImGui = -1.f;
        bool clampFPSInsideImGui = gImGuiInverseFPSClampInsideImGui > 0;
        ImGui::Checkbox("Clamp FPS when \"inside\" ImGui.",&clampFPSInsideImGui);
        if (clampFPSInsideImGui)    {
            if (gImGuiInverseFPSClampInsideImGui<=0) gImGuiInverseFPSClampInsideImGui = 1.f/60.f;
            float FPS = 1.f/gImGuiInverseFPSClampInsideImGui;
            if (ImGui::SliderFloat("FPS when \"inside\" ImGui",&FPS,5.f,60.f,"%.2f")) gImGuiInverseFPSClampInsideImGui = 1.f/FPS;
        }
        else gImGuiInverseFPSClampInsideImGui = -1.f;
        ImGui::Checkbox("Use dynamic FPS when \"inside\" ImGui.",&gImGuiDynamicFPSInsideImGui);
        ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Font options",collapsingHeaderFlags)) {
        //ImGui::Checkbox("Font Allow User Scaling", &ImGui::GetIO().FontAllowUserScaling);
        //if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","If true, CTRL + mouse wheel scales the window\n(or just its font size if child window).");
        ImGui::PushItemWidth(275);
        ImGui::DragFloat("Global Font Scale", &ImGui::GetIO().FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.2f"); // scale everything
        ImGui::PopItemWidth();
        if (ImGui::GetIO().FontGlobalScale!=1.f)    {
            ImGui::SameLine(0,10);
            if (ImGui::SmallButton("Reset##glFontGlobalScale")) ImGui::GetIO().FontGlobalScale = 1.f;
        }
        ImImpl_EditSdfParams(); // This lets you edit signed distance font params ONLY when they are used
        ImGui::TreePop();
        }

        // Some options ported from imgui_demo.cpp
        if (ImGui::TreeNodeEx("Window options",collapsingHeaderFlags)) {
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("Window Fill Alpha", &bg_alpha, 0.005f, -0.01f, 1.0f, bg_alpha < 0.0f ? "(default)" : "%.3f"); // Not exposing zero here so user doesn't "close" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
        ImGui::PopItemWidth();
        ImGui::PushItemWidth(275);
        ImGui::ColorEdit3("glClearColor",&clearColor.x);
        ImGui::PopItemWidth();
        if (clearColor.x!=defaultClearColor.x || clearColor.y!=defaultClearColor.y || clearColor.z!=defaultClearColor.z)    {
            ImGui::SameLine(0,10);
            if (ImGui::SmallButton("Reset##glClearColorReset")) clearColor = defaultClearColor;
        }

        ImGui::TreePop();
        }

        // imguistyleserializer test
        if (ImGui::TreeNodeEx("imguistyleserializer",collapsingHeaderFlags)) {
#       if (!defined(NO_IMGUISTYLESERIALIZER) && !defined(NO_IMGUISTYLESERIALIZER_SAVE_STYLE))
        ImGui::Text("Please modify the current style in:");
        ImGui::Text("ImGui Demo->Window Options->Style Editor");
        static bool loadCurrentStyle = false;
        static bool saveCurrentStyle = false;
        static bool resetCurrentStyle = false;
        loadCurrentStyle = ImGui::Button("Load Saved Style");
        saveCurrentStyle = ImGui::Button("Save Current Style");
        resetCurrentStyle = ImGui::Button("Reset Current Style To: ");
        ImGui::SameLine();
        static int styleEnumNum = 3;    // Gray style
        ImGui::PushItemWidth(135);
        ImGui::SelectStyleCombo("###StyleEnumCombo",&styleEnumNum);
        ImGui::PopItemWidth();
        if (ImGui::IsItemHovered()) {
            if   (styleEnumNum==ImGuiStyle_DefaultClassic)      ImGui::SetTooltip("%s","\"Default\"\nThis is the default\nclassic ImGui theme");
            else if (styleEnumNum==ImGuiStyle_DefaultDark)      ImGui::SetTooltip("%s","\"DefaultDark\"\nThis is the default\ndark ImGui theme");
            else if (styleEnumNum==ImGuiStyle_DefaultLight)      ImGui::SetTooltip("%s","\"DefaultLight\"\nThis is the default\nlight ImGui theme");
            else if (styleEnumNum==ImGuiStyle_Gray)   ImGui::SetTooltip("%s","\"Gray\"\nThis is the default\ntheme of this demo");
            else if (styleEnumNum==ImGuiStyle_BlackCodz01)   ImGui::SetTooltip("%s","\"BlackCodz01\"\nPosted by @codz01 here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_DarkCodz01) ImGui::SetTooltip("%s","\"DarkCodz01\"\nPosted by @codz01 here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_GrayCodz01)   ImGui::SetTooltip("%s","\"GrayCodz01\"\nPosted by @codz01 here:\nhttps://github.com/ocornut/imgui/issues/1607\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_DarkOpaque)   ImGui::SetTooltip("%s","\"DarkOpaque\"\nA dark-grayscale style with\nno transparency (by default)");
            else if (styleEnumNum==ImGuiStyle_Purple)   ImGui::SetTooltip("%s","\"Purple\"\nPosted by @fallrisk here:\nhttps://github.com/ocornut/imgui/issues/1607\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_Cherry)   ImGui::SetTooltip("%s","\"Cherry\"\nPosted by @r-lyeh here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_Soft) ImGui::SetTooltip("%s","\"Soft\"\nPosted by @olekristensen here:\nhttps://github.com/ocornut/imgui/issues/539\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_EdinBlack || styleEnumNum==ImGuiStyle_EdinWhite) ImGui::SetTooltip("%s","Based on an image posted by @edin_p\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_Maya) ImGui::SetTooltip("%s","\"Maya\"\nPosted by @ongamex here:\nhttps://gist.github.com/ongamex/4ee36fb23d6c527939d0f4ba72144d29\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_LightGreen) ImGui::SetTooltip("%s","\"LightGreen\"\nPosted by @ebachard here:\nhttps://github.com/ocornut/imgui/pull/1776\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_Design) ImGui::SetTooltip("%s","\"Design\"\nPosted by @usernameiwantedwasalreadytaken here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_Dracula) ImGui::SetTooltip("%s","\"Dracula\"\nPosted by @ice1000 here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_Greenish) ImGui::SetTooltip("%s","\"Greenish\"\nPosted by @dertseha here:\nhttps://github.com/ocornut/imgui/issues/1902\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_C64) ImGui::SetTooltip("%s","\"C64\"\nPosted by @Nullious here:\nhttps://gist.github.com/Nullious/2d598963b346c49fa4500ca16b8e5c67\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_PhotoStore) ImGui::SetTooltip("%s","\"PhotoStore\"\nPosted by @Derydoca here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_CorporateGreyFlat) ImGui::SetTooltip("%s","\"CorporateGreyFlat\"\nPosted by @malamanteau here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_CorporateGreyFramed) ImGui::SetTooltip("%s","\"CorporateGreyFramed\"\nPosted by @malamanteau here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_VisualDark) ImGui::SetTooltip("%s","\"VisualDark\"\nPosted by @mnurzia here:\nhttps://github.com/ocornut/imgui/issues/2529\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_SteamingLife) ImGui::SetTooltip("%s","\"SteamingLife\"\nPosted by @metasprite here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_SoftLife) ImGui::SetTooltip("%s","\"SoftLife\"\nA quick variation of the \"SteamingLife\" style.");
            else if (styleEnumNum==ImGuiStyle_GoldenBlack) ImGui::SetTooltip("%s","\"GoldenBlack\"\nPosted by @CookiePLMonster here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_Windowed) ImGui::SetTooltip("%s","\"Windowed\"\nBadly adapted from the Win98-DearImgui\ncustomization made by @JakeCoxon in his fork here:\nhttps://github.com/JakeCoxon/imgui-win98\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_OverShiftedBlack) ImGui::SetTooltip("%s","\"OverShiftedBlack\"\nPosted by @OverShifted here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_AieKickGreenBlue) ImGui::SetTooltip("%s","\"AieKickGreenBlue\"\nPosted by @aiekick here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_AieKickRedDark) ImGui::SetTooltip("%s","\"AieKickRedDark\"\nPosted by @aiekick here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");
            else if (styleEnumNum==ImGuiStyle_DeepDark) ImGui::SetTooltip("%s","\"DeepDark\"\nPosted by @janekb04 here:\nhttps://github.com/ocornut/imgui/issues/707\n(hope I can use it)");

        }

        ImGui::SameLine();
        static float hueShift = 0;
        ImGui::PushItemWidth(50);
        ImGui::DragFloat("HueShift##styleShiftHue",&hueShift,.005f,0,1,"%.2f");
        ImGui::PopItemWidth();
        if (hueShift!=0)   {
            ImGui::SameLine();
            if (ImGui::SmallButton("reset##styleReset")) {hueShift=0.f;}
        }
        const bool mustInvertColors = ImGui::Button("Invert Colors:##styleInvertColors");
        ImGui::SameLine();
        ImGui::PushItemWidth(50);
        static float invertColorThreshold = .1f;
        ImGui::DragFloat("Saturation Threshold##styleLumThres",&invertColorThreshold,.005f,0.f,0.5f,"%.2f");
        ImGui::PopItemWidth();
        if (mustInvertColors)  ImGui::ChangeStyleColors(ImGui::GetStyle(),invertColorThreshold);

        const char* pStyleFileName =  styleFileName;    // defined globally
        if (loadCurrentStyle)   {
#               if (defined(YES_IMGUIEMSCRIPTENPERSISTENTFOLDER) && !defined(NO_IMGUIHELPER) && !defined(NO_IMGUIHELPER_SERIALIZATION) && !defined(NO_IMGUIHELPER_SERIALIZATION_LOAD))
            if (ImGuiHelper::FileExists(styleFileNamePersistent)) pStyleFileName = styleFileNamePersistent;
#               endif //YES_IMGUIEMSCRIPTENPERSISTENTFOLDER & C
            if (!ImGui::LoadStyle(pStyleFileName,ImGui::GetStyle()))   {
                fprintf(stderr,"Warning: \"%s\" not present.\n",pStyleFileName);
            }
        }
        if (saveCurrentStyle)   {
#               ifdef YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
            pStyleFileName = styleFileNamePersistent;
#               endif //YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
            if (!ImGui::SaveStyle(pStyleFileName,ImGui::GetStyle()))   {
                fprintf(stderr,"Warning: \"%s\" cannot be saved.\n",pStyleFileName);
            }
            else {
#                   ifdef YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                ImGui::EmscriptenFileSystemHelper::Sync();
#                   endif //YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
            }
        }
        if (resetCurrentStyle)  {
            ImGui::ResetStyle(styleEnumNum,ImGui::GetStyle());
            if (hueShift!=0) ImGui::ChangeStyleColors(ImGui::GetStyle(),0.f,hueShift);
        }
#       else //NO_IMGUISTYLESERIALIZER
        ImGui::Text("%s","Excluded from this build.\n");
#       endif //NO_IMGUISTYLESERIALIZER
        ImGui::TreePop();
        }

        // imguifilesystem tests:
        if (ImGui::TreeNodeEx("imguifilesystem",collapsingHeaderFlags)) {
#       ifndef NO_IMGUIFILESYSTEM
        const char* startingFolder = "./";
        const char* optionalFileExtensionFilterString = "";//".jpg;.jpeg;.png;.tiff;.bmp;.gif;.txt";

        //------------------------------------------------------------------------------------------
        // 1 - ChooseFileDialogButton setup:
        //------------------------------------------------------------------------------------------
        ImGui::Text("Please choose a file: ");ImGui::SameLine();
        const bool browseButtonPressed = ImGui::Button("...");
        static ImGuiFs::Dialog fsInstance;
        const char* chosenPath = fsInstance.chooseFileDialog(browseButtonPressed,startingFolder,optionalFileExtensionFilterString);
        if (strlen(chosenPath)>0) {
            // A path (chosenPath) has been chosen right now. However we can retrieve it later using: fsInstance.getChosenPath()
        }
        if (strlen(fsInstance.getChosenPath())>0) ImGui::Text("Chosen path: \"%s\"",fsInstance.getChosenPath());

        //------------------------------------------------------------------------------------------
        // 2 - ChooseFolderDialogButton setup:
        //------------------------------------------------------------------------------------------
        ImGui::Text("Please choose a folder: ");ImGui::SameLine();
        const bool browseButtonPressed2 = ImGui::Button("...##2");
        static ImGuiFs::Dialog fsInstance2;
        const char* chosenFolder = fsInstance2.chooseFolderDialog(browseButtonPressed2,fsInstance2.getLastDirectory());
        if (strlen(chosenFolder)>0) {
            // A path (chosenFolder) has been chosen right now. However we can retrieve it later using: fsInstance2.getChosenPath()
        }
        if (strlen(fsInstance2.getChosenPath())>0) ImGui::Text("Chosen folder: \"%s\"",fsInstance2.getChosenPath());

        //------------------------------------------------------------------------------------------
        // 3 - SaveFileDialogButton setup:
        //------------------------------------------------------------------------------------------
        ImGui::Text("Please pretend to save the dummy file 'myFilename.png' to: ");ImGui::SameLine();
        const bool browseButtonPressed3 = ImGui::Button("...##3");
        static ImGuiFs::Dialog fsInstance3;
        const char* savePath = fsInstance3.saveFileDialog(browseButtonPressed3,"/usr/include","myFilename.png",".jpg;.jpeg;.png;.tiff;.bmp;.gif;.txt;.zip");//optionalFileExtensionFilterString);
        if (strlen(savePath)>0) {
            // A path (savePath) has been chosen right now. However we can retrieve it later using: fsInstance3.getChosenPath()
        }
        if (strlen(fsInstance3.getChosenPath())>0) ImGui::Text("Chosen save path: \"%s\"",fsInstance3.getChosenPath());

        //-----------------------------------------------------------------------------------------------------------
        // 4 - ChooseFileDialogButton setup using a read-only InputText sharing its internal buffer (derived from 1):
        //-----------------------------------------------------------------------------------------------------------
        static ImGuiFs::Dialog fsInstance4;
        ImGui::Text("Another File:");ImGui::SameLine();
        ImGui::InputText("###dummyStuffForID",(char*)fsInstance4.getChosenPath(),ImGuiFs::MAX_PATH_BYTES,ImGuiInputTextFlags_ReadOnly);ImGui::SameLine();
        const bool browseButtonPressed4 = ImGui::Button("...##4");
        fsInstance4.chooseFileDialog(browseButtonPressed4,fsInstance4.getLastDirectory());


#       else //NO_IMGUIFILESYSTEM
        ImGui::Text("%s","Excluded from this build.\n");
#       endif //NO_IMGUIFILESYSTEM
        ImGui::TreePop();
        }

        // imguitinyfiledialogs tests (-->> not portable to emscripten and mobile platforms I guess <<--):
#       ifdef YES_IMGUITINYFILEDIALOGS
        if (ImGui::TreeNodeEx("imguitinyfiledialogs (yes_addon)",collapsingHeaderFlags)) {

            const char* startingFolder = "./";
            const char* optionalFileFilterPatterns[7] = {"*.jpg","*.jpeg","*.png","*.tiff","*.bmp","*.gif","*.txt"};
            static ImVector<char> path0;  // The vector is necessary for "aAllowMultipleSelects".
                                          // I don't want to use an ImString here (too easy and some users might exclude it)
            if (path0.size()==0) {path0.resize(1);path0[0]='\0';}
            static char paths[2][1024] = {"",""};   // tinydialogs hard codes the MAX_PATH to 1024 AFAICS

            //------------------------------------------------------------------------------------------
            // 1 - ChooseFileDialogButton setup:
            //------------------------------------------------------------------------------------------
            ImGui::Text("Please choose a file: ");ImGui::SameLine();
            if (ImGui::Button("...##tinyfiledialogs1")) {
                const char* chosenPath =
                tfd::openFileDialog ("Choose a file",                   // aTitle
                                     startingFolder,                    // aDefaultPathAndFile
                                     0,//7,                             // aNumOfFilterPatterns
                                     NULL,//optionalFileFilterPatterns, // char const * const * const aFilterPatterns , /* NULL {"*.jpg","*.png"} */
                                     NULL,                              // char const * const aSingleFilterDescription , /* NULL | "image files" */
                                     0//1                               // aAllowMultipleSelects 0 or 1 (returned paths are chained using the '|' char...)
                                     ) ;
                if (chosenPath) {
                    path0.resize(strlen(chosenPath)+1);
                    strcpy(&path0[0],chosenPath);
                }
            }
            if (path0[0]!='\0') ImGui::TextWrapped("Chosen path(s): \"%s\"",&path0[0]);

            //------------------------------------------------------------------------------------------
            // 2 - ChooseFolderDialogButton setup:
            //------------------------------------------------------------------------------------------
            ImGui::Text("Please choose a folder: ");ImGui::SameLine();
            if (ImGui::Button("...##tinyfiledialogs2")) {
                const char* chosenPath =
                tfd::selectFolderDialog ("Choose a folder",             // aTitle
                                     startingFolder                     // aDefaultPath
                                     ) ;
                if (chosenPath) strcpy(&paths[0][0],chosenPath);
            }
            if (paths[0][0]!='\0') ImGui::Text("Chosen folder: \"%s\"",&paths[0][0]);

            //------------------------------------------------------------------------------------------
            // 3 - SaveFileDialogButton setup:
            //------------------------------------------------------------------------------------------
            ImGui::Text("Please pretend to save the dummy file 'myFilename.png' to: ");ImGui::SameLine();
            if (ImGui::Button("...##tinyfiledialogs3")) {
                const char* chosenPath =
                tfd::saveFileDialog ("Choose a save path",              // aTitle
                                     "myFilename.png",                  // aDefaultPathAndFile
                                     7,                                 // aNumOfFilterPatterns
                                     optionalFileFilterPatterns,        // char const * const * const aFilterPatterns , /* NULL {"*.jpg","*.png"} */
                                     NULL                               // char const * const aSingleFilterDescription , /* NULL | "image files" */
                                     ) ;
                if (chosenPath) strcpy(&paths[1][0],chosenPath);
            }
            if (paths[1][0]!='\0') ImGui::Text("Chosen save path: \"%s\"",&paths[1][0]);

            //------------------------------------------------------------------------------------------
            // 4 - ChooseColorButton setup: (this does not seem to work as expected on Linux)
            //------------------------------------------------------------------------------------------
            static ImVec4 chosenColor(1,1,1,1);
            ImGui::AlignTextToFramePadding();ImGui::Text("Please choose a color:");ImGui::SameLine();
            ImGui::PushID(20);  // (I reuse ImGui::ColorButton(...) below without pushing any ID)
            if (ImGui::ColorButton("###MyColorButtonControl",chosenColor))    {
                unsigned char aoResultRGB[3] = {(unsigned char)(chosenColor.x*255.f),(unsigned char)(chosenColor.y*255.f),(unsigned char)(chosenColor.z*255.f)};

                char const * hexString =            /* returns the hexcolor as a string "#FF0000" */
                tfd::colorChooser(
                            "Choose a color",       //aTitle , /* "" */
                            NULL,                   //char const * const aDefaultHexRGB , /* NULL or "#FF0000" */
                            aoResultRGB,            /* { 0 , 255 , 255 } used only if aDefaultHexRGB is NULL */
                            aoResultRGB             /* { 0 , 0 , 0 } aDefaultRGB and aoResultRGB can be the same array */
                            );
                if (hexString) {
                    chosenColor.x = ((float)aoResultRGB[0])/255.0f;
                    chosenColor.y = ((float)aoResultRGB[1])/255.0f;
                    chosenColor.z = ((float)aoResultRGB[2])/255.0f;
                }
            }
            ImGui::PopID();
            ImGui::SameLine();ImGui::TextDisabled(" (Does this work or not?)");
            ImGui::TreePop();
        }
#       endif //YES_IMGUITINYFILEDIALOGS

        // Datetime Test:
        if (ImGui::TreeNodeEx("imguidatetime",collapsingHeaderFlags)) {
#       ifndef NO_IMGUIDATETIME
        /*struct tm {
  int tm_sec;			 Seconds.	[0-60] (1 leap second)
  int tm_min;			 Minutes.	[0-59]
  int tm_hour;			 Hours.	[0-23]
  int tm_mday;			 Day.		[1-31]
  int tm_mon;			 Month.	[0-11]
  int tm_year;			 Year	- 1900.
  int tm_wday;			 Day of week.	[0-6]
  int tm_yday;			 Days in year.[0-365]
  };*/
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Choose a date:");
        ImGui::SameLine();
        static tm myDate={};       // IMPORTANT: must be static! (plenty of compiler warnings here if we write: static tm myDate={0}; Is there any difference?)
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x*0.5f);
        if (ImGui::InputDate("Date ##MyDate",myDate,"%d/%m/%Y")) {
            // A new date has been chosen
            //fprintf(stderr,"A new date has been chosen exacty now: \"%.2d-%.2d-%.4d\"\n",myDate.tm_mday,myDate.tm_mon+1,myDate.tm_year+1900);
        }
        ImGui::Text("Chosen date: \"%.2d-%.2d-%.4d\"",myDate.tm_mday,myDate.tm_mon+1,myDate.tm_year+1900);

        ImGui::Spacing();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Choose another date with time:");
        ImGui::SameLine();
        static tm myDate2={};       // IMPORTANT: must be static! (plenty of compiler warnings here if we write: static tm myDate={0}; Is there any difference?)
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x*0.5f);
        if (ImGui::InputDateTime("##MyDate2",myDate2,"%d/%m/%Y %H:%M")) {
            // A new date has been chosen
            //fprintf(stderr,"A new date has been chosen exacty now: \"%.2d-%.2d-%.4d\"\n",myDate2.tm_mday,myDate2.tm_mon+1,myDate2.tm_year+1900);
        }
        ImGui::Text("Chosen date2: \"%.2d-%.2d-%.4d\"",myDate2.tm_mday,myDate2.tm_mon+1,myDate2.tm_year+1900);

#       else       //NO_IMGUIDATETIME
        ImGui::Text("%s","Excluded from this build.\n");
#       endif      //NO_IMGUIDATETIME
        ImGui::TreePop();
        }

        // imguivariouscontrols
        if (ImGui::TreeNodeEx("imguivariouscontrols",collapsingHeaderFlags)) {
#       ifndef NO_IMGUIVARIOUSCONTROLS

        ImGui::Spacing();ImGui::Separator();ImGui::Text("Some old stuff:");ImGui::Separator();

        ImGui::Spacing();
        if (ImGui::TreeNodeEx("Some (old) ProgressBar Stuff/Experiments:"))     {
            // ProgressBar Test:
            ImGui::TestProgressBar();
            ImGui::TreePop();
        }

        ImGui::Spacing();
        if (ImGui::TreeNodeEx("Some (old) Color Stuff/Experiments:"))     {
            // ColorChooser Test:
            static ImVec4 chosenColor(1,1,1,1);
            static bool openColorChooser = false;
            ImGui::AlignTextToFramePadding();ImGui::Text("Please choose a color:");ImGui::SameLine();
            openColorChooser|=ImGui::ColorButton("color button",chosenColor);
            //if (openColorChooser) chosenColor.z=0.f;
            if (ImGui::ColorChooser(&openColorChooser,&chosenColor)) {
                // choice OK here
            }
            // ColorComboTest:
            static ImVec4 chosenColor2(1,1,1,1);
            if (ImGui::ColorCombo("MyColorCombo",&chosenColor2))
            {
                // choice OK here
            }
            ImGui::TreePop();
        }

        ImGui::Spacing();
        if (ImGui::TreeNodeEx("Some (old) Menu Stuff/Experiments:"))     {
            // PopupMenuSimple Test:
            // Recent Files-like menu
            static const char* recentFileList[] = {"filename01","filename02","filename03","filename04","filename05","filename06","filename07","filename08","filename09","filename10"};
            static ImGui::PopupMenuSimpleParams pmsParams;
            /*const bool popupMenuButtonClicked = */ImGui::Button("Right-click me##PopupMenuSimpleTest");
            pmsParams.open|= ImGui::GetIO().MouseClicked[1] && ImGui::IsItemHovered(); // RIGHT CLICK on the last widget
            //popupMenuButtonClicked;    // Or we can just click the button
            const int selectedEntry = ImGui::PopupMenuSimple(pmsParams,recentFileList,(int) sizeof(recentFileList)/sizeof(recentFileList[0]),5,true,"RECENT FILES");
            static int lastSelectedEntry = -1;
            if (selectedEntry>=0) {
                // Do something: clicked recentFileList[selectedEntry].
                // Good in most cases, but here we want to persist the last choice because this branch happens only one frame:
                lastSelectedEntry = selectedEntry;
            }
            if (lastSelectedEntry>=0) {ImGui::SameLine();ImGui::Text("Last selected: %s\n",recentFileList[lastSelectedEntry]);}


            // Single column popup menu with icon support. It disappears when the mouse goes away. Never tested.
            // User is supposed to create a static instance of it, add entries once, and then call "render()".
            static ImGui::PopupMenu pm;
            if (pm.isEmpty())   {
                pm.addEntryTitle("Single Menu With Images");
                char tmp[1024];ImVec2 uv0(0,0),uv1(0,0);
                for (int i=0;i<9;i++) {
                    strcpy(tmp,"Image Menu Entry ");
                    sprintf(&tmp[strlen(tmp)],"%d",i+1);
                    uv0 = ImVec2((float)(i%3)/3.f,(float)(i/3)/3.f);
                    uv1 = ImVec2(uv0.x+1.f/3.f,uv0.y+1.f/3.f);

                    pm.addEntry(tmp,reinterpret_cast<void*>(myImageTextureId2),uv0,uv1);
                }

            }
            static bool trigger = false;
            trigger|=ImGui::Button("Press me for a menu with images##PopupMenuWithImagesTest");
            /*const int selectedImageMenuEntry =*/ pm.render(trigger);   // -1 = none
            ImGui::TreePop();
        }

        // Based on the code from: https://github.com/Roflraging
        ImGui::Spacing();
        if (ImGui::TreeNodeEx("Some (old) InputText Stuff/Experiments:"))     {
            // Fast copy/cut/paste menus
            static char buf[512]="Some sample text";
            ImGui::InputTextMultiline("Right click to have\na (non-functional)\ncopy/cut/paste menu\nin one line of code##TestCopyCutPaste",buf,512);
            const int cutCopyOrPasteSelected = ImGui::PopupMenuSimpleCopyCutPasteOnLastItem();
            if (cutCopyOrPasteSelected>=0)  {
                // Here we have 0 = cut, 1 = copy, 2 = paste
                // However ImGui::PopupMenuSimpleCopyCutPasteOnLastItem() can't perform these operations for you
                // and it's not trivial at all... at least I've got no idea on how to do it!
                // Moreover, the selected text seems to lose focus when the menu is selected...
            }

            ImGui::Text("InputTextMultiline with horizontal scrolling:");
            static char buffer[1024] = "Code posted by Roflraging to the ImGui Issue Section (https://github.com/ocornut/imgui/issues/383).";
            const float height = 60;
            ImGui::PushID(buffer);
            ImGui::InputTextMultilineWithHorizontalScrolling("ITMWHS", buffer, 1024, height);   // Note that now the label is not displayed ATM
            ImGui::PopID();

            ImGui::Spacing();
            ImGui::Text("Same as above with a context-menu that should work (more or less):");
            static char buffer2[1024] = "Code posted by Roflraging to the ImGui Issue Section (https://github.com/ocornut/imgui/issues/383).";
            ImGui::PushID(buffer2);
            static bool popup_open = false;static int threeStaticInts[3]={0,0,0};
            ImGui::InputTextMultilineWithHorizontalScrollingAndCopyCutPasteMenu("ITMWHS2", buffer2, 1024, height,popup_open,threeStaticInts);
            ImGui::PopID();
            ImGui::TreePop();
        }

        ImGui::Spacing();ImGui::Separator();ImGui::Text("Extended Buttons And Checkboxes:");ImGui::Separator();
        if (ImGui::TreeNode("Extended Buttons And Checkboxes##VCEB"))  {
            // Image Button
            ImGui::Text("(Animated) Image Buttons/Gifs:");
            ImGui::ImageButtonWithText(reinterpret_cast<ImTextureID>(myImageTextureId2),"MyImageButtonWithText",ImVec2(16,16),ImVec2(0,0),ImVec2(0.33334f,0.33334f));

            // Animated Image Buttons/Animated Gifs
#           ifndef NO_IMGUIVARIOUSCONTROLS_ANIMATEDIMAGE
            // One instance per image, but it can feed multiple widgets
            static ImGui::AnimatedImage gif(myImageTextureId2,64,64,9,3,3,30,true);
            //static ImGui::AnimatedImage gif("extra/awesome.gif",true);
            ImGui::SameLine();
            gif.render();
            ImGui::SameLine();
            gif.renderAsButton("myButton123",ImVec2(-.5f,-.5f));    // Negative size multiplies the 'native' gif size
#        endif //NO_IMGUIVARIOUSCONTROLS_ANIMATEDIMAGE

            // ColoredButtonV1: code posted by @ocornut here:
            // https://github.com/ocornut/imgui/issues/4722
            ImGui::Spacing();
            ImGui::Text("ColorButtonV1 (by @ocornut: hope we can use it)");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Code posted by @ocornut here:\nhttps://github.com/ocornut/imgui/issues/4722");
            // [Button rounding depends on the FrameRounding Style property (but can be overridden with the last argument)]
            const float cbv1width = ImGui::GetContentRegionAvail().x*0.45f;
            ImGui::ColoredButtonV1("Hello##ColoredButtonV1Hello", ImVec2(cbv1width, 0.0f), IM_COL32(255, 255, 255, 255), IM_COL32(200, 60, 60, 255), IM_COL32(180, 40, 90, 255));
            ImGui::SameLine();
            ImGui::ColoredButtonV1("You##ColoredButtonV1You", ImVec2(cbv1width, 0.0f), IM_COL32(255, 255, 255, 255), IM_COL32(50, 220, 60, 255), IM_COL32(69, 150, 70, 255),10.0f); // FrameRounding in [0.0,12.0]

            // Check Buttons
            ImGui::Spacing();
            ImGui::AlignTextToFramePadding();ImGui::Text("Check Buttons:");
            static bool checkButtonState1=false;
            if (ImGui::CheckButton("CheckButton",&checkButtonState1)) {/*checkButtonState1 changed*/}
            ImGui::SameLine();
            static bool checkButtonState2=false;
            if (ImGui::SmallCheckButton("SmallCheckButton",&checkButtonState2)) {/*checkButtonState2 changed*/}

            // CheckBox Styled
            ImGui::Spacing();
            ImGui::AlignTextToFramePadding();ImGui::Text("CheckBox Styled:");
            static bool checkStyled[2] = {false,true};
            // Default look and size
            ImGui::CheckboxStyled("Checkbox Styled 1 (default style)",&checkStyled[0]);
            // Custom look and size
            static const ImU32 optionalEightColors[8] = {
                IM_COL32(220,220,220,255),IM_COL32(255,255,255,255),    // on_circle (normal and hovered)
                IM_COL32(80,80,80,255),IM_COL32(100,100,100,255),       // off_circle (normal and hovered)
                IM_COL32(60,120,60,255),IM_COL32(80,155,80,255),        // on_bg (normal and hovered)
                IM_COL32(60,60,60,255),IM_COL32(80,80,80,255)           // off_bg (normal and hovered)
            };
            const ImVec2 checkBoxSizeScale(1.5f,2.f);   // (checkBoxSizeScale.y max is 2.f)
            const float checkBoxRounding = 6.f;         // -1 defaults to style.WindowRounding. Note that is scales with the font height)
            ImGui::CheckboxStyled("Checkbox Styled 2 (custom style)",&checkStyled[1],optionalEightColors,checkBoxSizeScale,checkBoxRounding);

            // CheckboxFlags Overload
            ImGui::Spacing();
            ImGui::AlignTextToFramePadding();ImGui::Text("CheckBoxFlags Overload:");
            static int numFlags=16;
            static int numRows=2;
            static int numColumns=3;
            ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.2f);
            ImGui::SliderInt("Flags##CBF_Flags",&numFlags,1,20);ImGui::SameLine();
            ImGui::SliderInt("Rows##CBF_Rows",&numRows,1,4);ImGui::SameLine();
            ImGui::SliderInt("Columns##CBF_Columns",&numColumns,1,5);
            ImGui::PopItemWidth();

            static unsigned int cbFlags = (unsigned int)  128+32+8+1;
            static const unsigned int cbAnnotationFlags = 0;//132;   // Optional (default is zero = no annotations)
            int flagIndexHovered = -1;  // Optional
            ImGui::CheckboxFlags("Flags###CBF_Overload",&cbFlags,numFlags,numRows,numColumns,cbAnnotationFlags,&flagIndexHovered);
            if (flagIndexHovered!=-1) {
                // Test: Manual positional tooltip
                ImVec2 m = ImGui::GetIO().MousePos;
                ImGui::SetNextWindowPos(ImVec2(m.x, m.y+ImGui::GetTextLineHeightWithSpacing()));
                ImGui::Begin("CBF_Overload_Tooltip", NULL, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
                ImGui::Text("flag %d. Hold shift\nwhile clicking to toggle it",flagIndexHovered);
                ImGui::End();
            }
            // BUG: This is completely wrong (both x and y position):
            //ImGui::SameLine(0,0);ImGui::Text("%s","Test");    // (I don't know how to get this fixed)

            ImGui::TreePop();
        }

        ImGui::Spacing();ImGui::Separator();ImGui::Text("AutoCompletion Stuff");ImGui::Separator();
        if (ImGui::TreeNodeEx("AutoCompletion Stuff (UP/DOWN/TAB keys):"))  {
            // Bad implementation: users think they can click on the autocompletion-menu, instead of using TAB+ARROWS

            static const int bufferSize = 84;   // Mandatory [ImGui::InputText(...) needs it]
            // Customizable section (we will use ImVector<char[bufferSize]> here to store our autocompletion entries; it can be easily changed to std::vector<std::string> or similiar):
            typedef struct _TMP {
                // MANDATORY! The same signature as ImGui::Combo(...)
                static bool ItemGetter(void* data,int i,const char** txt)   {
                    IM_ASSERT(data);
                    const ImVector<char[bufferSize]>& v = *((const ImVector<char[bufferSize]>*)data);
                    if (i>=0 && i<v.size()) {*txt=v[i];return true;}
                    return false;
                }
                // This is optional in InputTextWithAutoCompletion(...) (all at the user side), but mandatory in InputComboWithAutoCompletion(...)
                static bool ItemInserter(void* data,int pos,const char* txt)  {
                    IM_ASSERT(data && txt && strlen(txt)<bufferSize);
                    ImVector<char[bufferSize]>& v = *((ImVector<char[bufferSize]>*)data);
                    const int size = v.size();
                    if (pos<0) pos=0;else if (pos>size) pos=size;

                    v.resize(size+1);
                    for (int i=size;i>pos;--i) strcpy(v[i],v[i-1]);
                    strcpy(v[pos],txt);

                    return true;
                }
                // This is optional in InputComboWithAutoCompletion(...) (not needed by InputTextWithAutoCompletion(...))
                static bool ItemDeleter(void* data,int pos)  {
                    IM_ASSERT(data);
                    ImVector<char[bufferSize]>& v = *((ImVector<char[bufferSize]>*)data);
                    const int size = v.size();
                    if (pos<0 || pos>size) return false;

                    for (int i=pos,iSz=size-1;i<iSz;++i) strcpy(v[i],v[i+1]);
                    v.resize(size-1);

                    return true;
                }
                // This is optional in InputComboWithAutoCompletion(...) (not needed by InputTextWithAutoCompletion(...))
                // This method can be built out of ItemDeleter(...) and ItemInserter(...) [but it's still good to notify user that a rename has occurred]
                static bool ItemRenamer(void* data,int posOld,int posNew,const char* txtNew)  {
                    // We must move item at posOld to position posNew, and change its text to txtNew
                    IM_ASSERT(data && txtNew && strlen(txtNew)<bufferSize);
                    ImVector<char[bufferSize]>& v = *((ImVector<char[bufferSize]>*)data);
                    const int size = v.size();
                    if (posOld<0 || posOld>size) return false;
                    if (posNew<0 || posNew>size) return false;
                    // worst implementation possible here:
                    bool ok = ItemDeleter(data,posOld);
                    if (ok) ok = ItemInserter(data,posNew,txtNew);
                    return ok;
                }
            } TMP;

            ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.275f);
            // InputTextWithAutoCompletion:
            {
                // Mandatory stuff
                static char buf[bufferSize];
                static ImGui::InputTextWithAutoCompletionData bufData(ImGuiInputTextFlags_CharsUppercase);  // Only ImGuiInputTextFlags_Chars... flags are allowed here
                static ImVector<char[bufferSize]> autocompletionEntries;    // The type here depends on our TMP struct
                // [Optional] Bad init, but very comfortable to start with something
                if (!bufData.isInited()) {
                    const char* entries[] = {"WATERMELON","STRAWBERRY","APRICOT","APPLE","KIWI","CHERRY","PAPAYA",
                                             "LEMON","PEACH","PEAR","PINEAPPLE"};
                    const int numEntries = (int) sizeof(entries)/sizeof(entries[0]);
                    for (int i=0;i<numEntries;i++)  {
                        // This helper call should ensure sorting + duplicate elimination
                        ImGui::InputTextWithAutoCompletionData::HelperInsertItem(entries[i],TMP::ItemGetter,TMP::ItemInserter,autocompletionEntries.size(),&autocompletionEntries);
                    }
                    /* Otherwise we could have:
                    // Important: entries must be sorted alphabetically
                    TMP::ItemInserter(&autocompletionEntries,0,"APPLE");
                    TMP::ItemInserter(&autocompletionEntries,1,"APRICOT");
                    TMP::ItemInserter(&autocompletionEntries,2,"CHERRY");
                    // ... and so on
                    */
                    // [Optional] user can handle bufData.currentAutocompletionItemIndex
                    // bufData.currentAutocompletionItemIndex is owned by the user (for ImGui::InputTextWithAutoCompletion(...) only).
                    // When !=-1, the specified item is displayed in a different way in the autocompletion menu.
                    // bufData.currentAutocompletionItemIndex = 2;
                }

                if (ImGui::InputTextWithAutoCompletion("Fruits##AutoCompleteIT",buf,bufferSize,&bufData,TMP::ItemGetter,autocompletionEntries.size(),(void*)&autocompletionEntries) && buf[0]!='\0')    {
                    // Return has been pressed and buf is valid
                    if (bufData.getItemPositionOfReturnedText()>=0)  {
                        // The entered text must be inserted at that position
                        TMP::ItemInserter(&autocompletionEntries,bufData.getItemPositionOfReturnedText(),buf);
                        // bufData.currentAutocompletionItemIndex = bufData.getItemPositionOfReturnedText();
                    }
                    // else if (bufData.getItemIndexOfReturnedText()>=0)    {
                    // User has entered an existing autocomplete item that can be retrieved at position: bufData.getItemIndexOfReturnedText()
                    // bufData.currentAutocompletionItemIndex = getItemIndexOfReturnedText();
                    //}
                    buf[0]='\0';  // clear
                    ImGui::SetKeyboardFocusHere(-1);    // So we keep typing
                }
            }
            ImGui::SameLine(0,ImGui::GetWindowWidth()*0.05f);
            // InputComboWithAutoCompletion:
            {
                // Mandatory stuff
                static int current_item=-1;
                static ImGui::InputComboWithAutoCompletionData bufData;
                static ImVector<char[bufferSize]> autocompletionEntries;    // The type here depends on our TMP struct
                // [Optional] Bad init, but very comfortable to start with something
                if (!bufData.isInited()) {
                    const char* entries[] = {"black","blue","green","ivory","pink","red","white","yellow"};
                    const int numEntries = (int) sizeof(entries)/sizeof(entries[0]);
                    for (int i=0;i<numEntries;i++)  {
                        // This helper call should ensure sorting + duplicate elimination
                        ImGui::InputTextWithAutoCompletionData::HelperInsertItem(entries[i],TMP::ItemGetter,TMP::ItemInserter,autocompletionEntries.size(),&autocompletionEntries);
                    }
                    // We CAN'T handle bufData.currentAutocompletionItemIndex for Combos, because we have:
                    current_item = 2;
                }

                //ImGui::SetNextItemWidth(150.f);
                if (ImGui::InputComboWithAutoCompletion("Colors##AutoCompleteIT",&current_item,bufferSize,&bufData,
                                                        TMP::ItemGetter,TMP::ItemInserter,TMP::ItemDeleter,TMP::ItemRenamer,   // TMP::ItemDeleter and TMP::ItemRenemer can be NULL
                                                        autocompletionEntries.size(),(void*)&autocompletionEntries))    {
                    // something has changes (the Combo selected item, and/or an insert/delete operation
                }
                // For this particular widget ImGui::IsItemHovered() does not always work as expected. Workaround:
                //if (bufData.isItemHovered()) ImGui::SetTooltip("%s","InputComboWithAutoCompletion tooltip");
            }
            ImGui::PopItemWidth();

            ImGui::TreePop();
        }


        ImGui::Spacing();ImGui::Separator();ImGui::Text("Image with zoom (CTRL+MW) and pan (RMB drag):");ImGui::Separator();
        static float zoom = 1.f;static ImVec2 pan(.5f,.5f);
        // This requires     ImGui::GetIO().FontAllowUserScaling = false;
        ImGui::ImageZoomAndPan(reinterpret_cast<ImTextureID>(myImageTextureId2),ImVec2(0,150),1.f,zoom,pan);    // aspect ratio can be aero for stretch mode

        ImGui::Spacing();ImGui::Separator();ImGui::Text("Collapsable Header with buttons [ImGui::AppendTreeNodeHeaderButtons(...)]");ImGui::Separator();
        // Nobody will use this, it's too complicated to set up. However:
        { // start scope
            static bool displayButtonsOnlyOnItemHovering = false;     // tweakable
            static bool closed = false;                               // button sensor (this can be static even if it's not a toggle-button just because we don't display the header at all as soon as 'close' becomes true...)
            bool paste = false, copy = false;                         // button sensors (we won't use them)
            if (!closed)    {
                static bool myTreeNodeIsOpen = false;   // 'static' here, just to reuse its address as id...
                const void* ptr_id = &myTreeNodeIsOpen;
                const float curPosX = ImGui::GetCursorPosX();   // used for clipping
                ImGui::BeginGroup();    // Not sure grouping is strictly necessary here
                myTreeNodeIsOpen = ImGui::TreeNodeEx(ptr_id,collapsingHeaderFlags|ImGuiTreeNodeFlags_AllowItemOverlap,"Collapsable %d",1);
                const bool isCollapsableHeaderHovered = ImGui::IsItemHovered();
                if (!displayButtonsOnlyOnItemHovering || isCollapsableHeaderHovered)
                {
                    static const char* tmpTooltips[2] = {"show these\nbuttons always","display these buttons only when\nthe collapsable header is hovered"};

                    const int rv = ImGui::AppendTreeNodeHeaderButtons(ptr_id,curPosX,
                        6,                            // Num Buttons + Num Separators
                        &closed,"delete",NULL,0,      // Button 0 (far-right) quartet:         &pressed | tooltip | single glyph as const char* (if NULL it's a close button) | isToggleButton?1:0
                        NULL,NULL,NULL,0,             // Button 1 (separator)
                        &paste,"paste","v",0,         // Button 2 (second far-right) quartet:  &pressed | tooltip | single glyph as const char* (if NULL it's a close button) | isToggleButton?1:0
                        &copy,"copy","^",0,           // Button 3 (third far-right) quartet:   &pressed | tooltip | single glyph as const char* (if NULL it's a close button) | isToggleButton?1:0
                        NULL,NULL,NULL,0,             // Button 4 (separator)
                        &displayButtonsOnlyOnItemHovering,tmpTooltips[displayButtonsOnlyOnItemHovering?0:1],"h",1   // Button 5 well, same as above... except that it's togglable, and we use a static boolean
                    );
                    // rv can be: -1 => No button is hovered or clicked | [0,numButtons-1] => buttons[rv] has been clicked | [numButtons,2*numButtons-1] => buttons[rv-numButtons] is hovered
                    if ((displayButtonsOnlyOnItemHovering || isCollapsableHeaderHovered) && rv==-1) ImGui::SetTooltip("%s","Optional collapsing\nheader tooltip");
                }
                if (myTreeNodeIsOpen) {
                    // (optional) Fill the header with data within tree node indent
                }
                if (myTreeNodeIsOpen) ImGui::TreePop();   // Mandatory! When we want to close the indent (before or after filling the header with data)
                if (myTreeNodeIsOpen) {
                    // (optional) Fill the header with data without tree node indent
                    static ImVec4 color(1,1,1,1);ImGui::ColorEdit4("MyColor##AppendTreeNodeHeaderButtonsMyColor",&color.x);
                }
                ImGui::EndGroup();    // Not sure grouping is strictly necessary here
            }
            else if (ImGui::Button("Reset collapsable header##AppendTreeNodeHeaderButtonsReset")) closed = false;
        } // end scope        

        ImGui::Spacing();ImGui::Separator();ImGui::Text("Graph Widgets:");ImGui::Separator();
        {
            if (ImGui::TreeNode("ImGui::PlotHistogram(...) overload###ImGui::PlotHistogram(...) overloadCH")) {

                static const float values[][5]={
                    {2.f,   -1.f,   0.5f,   1.f,    0.25f},
                    {1.8f,  -1.5f,  0.75f,  1.3f,   -0.25f},
                    {1.f,   -2.75f, 1.2f,   0.8f,   0.5f},
                };
                static const int num_histograms = sizeof(values)/sizeof(values[0]);
                static const int num_values = sizeof(values[0])/sizeof(values[0][0]);
                //IM_ASSERT(num_histograms==3 && num_values==5);
                // It's not straight-forward, but 'values' CAN'T be simply converted to a 'const float**':
                static const float *ppValues[num_histograms] = {values[0],values[1],values[2]};
                //for (int i=0;i<num_histograms;i++) ppValues[i]=values[i];

                ImGui::PlotHistogram("PlotHistogram(...)\nRange[-2,2]###MHSGM",ppValues,num_histograms,num_values,0,"Multi-Histogram",-2.f,2.f,ImVec2(0,80),sizeof(float),10.f);

                ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.2f);
                {
                    // Using the same signature as the multi-PlotHistogram used above
                    // for a single histogram is a bit tricky:
                    static const float values[1][5]={{1,2,3,4,5}};
                    static const int num_values = sizeof(values[0])/sizeof(values[0][0]);
                    static const float *ppValues[1]={values[0]};
                    ImGui::PlotHistogram("Range[-0.2,4]###MHSGM2",ppValues,1,num_values,0,NULL,0.2f,4.f,ImVec2(0,80));
                }
                ImGui::SameLine();
                {
                    // So we provide ImGui::PlotHistogram2(...) with the same signature of (one of) the
                    // dafault Dear ImGui::PlotHistogram(...) methods.
                    static const float values[]={-1,-2,-3,-4,-5};
                    static const int num_values = sizeof(values)/sizeof(values[0]);
                    ImGui::PlotHistogram2("Range[-4.5,-2.1]###MHSGM3",values,num_values,0,NULL,-4.5f,-2.1f,ImVec2(0,80));
                }
                ImGui::PopItemWidth();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("ImGui::PlotCurve(...)###ImGui::PlotCurve(...) overloadCH")) {
                // [Experimental] ImGui::PlotCurve(...)
                typedef struct _MyCurveGetterStruct {
                    static float GetCurve(void* ,float x,int curve_idx) {
                        if (curve_idx==0) return x-x*x*0.5f+x*x*x/6.f;
                        else if (curve_idx==1) return -x+x*x*x/6.f;
                        else if (curve_idx==2) return (x*x)-10.f;
                        return 1.f; // Never happens
                    }
                } MyCurveGetterStruct;
                // Tip: To avoid stretching one axis you can set the upper limit of the 'rangeX' argument to FLT_MAX
                ImGui::PlotCurve("PlotCurve(...)\nRange:\nY[-10,10]\nX[-5,5]###MPC",&MyCurveGetterStruct::GetCurve,NULL,3,"Multi-Curve",ImVec2(-10.f,10.f),ImVec2(-5.f,5.f),ImVec2(0,80));


                ImGui::TreePop();
            }

            // ImGui::PlotMultiLines(...) and ImGui::PlotMultiHistograms(...)
            // from here: https://github.com/ocornut/imgui/issues/632
            // (made by @JaapSuter and @maxint)
            // are available too (but no example so far... I'm too lazy to code it)
        }

        ImGui::Spacing();ImGui::Separator();ImGui::Text("Generic TreeView Implementation:");//ImGui::Separator();
        {
            // Actually we can use less than 5 lines of code to setup and run a very basic TreeView...
            // [the single line: static ImGui::TreeView tv;tv.render(); works, but it's without any item...]
            // but we need to perform some tests here...

            static ImGui::TreeView tv;
            if (!tv.isInited()) {
                // Here we add some optional callbacks (can be commented out):
                if (myImageTextureId2) tv.setTreeViewNodeDrawIconCb(&MyTreeViewNodeDrawIconCallback);
                // there are other callbacks we can set... expecially the popup menu that defaults to the "testing" one...
                //tv.setTreeViewNodeCreationDelationCb(&MyTreeViewNodeCreationDelationCallback);
            }


            static bool resetTv = false;    // triggered by an ImGui::Button below...
            if (!tv.isInited() || resetTv) {                
                if (resetTv)    {
                    resetTv = false;
                    tv.clear();
                    tv.selectionMode=ImGui::TreeViewNode::MODE_ALL;
                    tv.allowMultipleSelection=false;
                    tv.checkboxMode=ImGui::TreeViewNode::MODE_NONE;
                    tv.allowAutoCheckboxBehaviour=true;
                    tv.inheritDisabledLook=true;
                }

                // Here we add some nodes for testing:
                ImGui::TreeViewNode* n = tv.addRootNode(ImGui::TreeViewNodeData("Some nations","Some nations here"));
                //n->addState(ImGui::TreeViewNode::STATE_COLOR1);    // configurable color set
                n->setUserId(1);    // (optional) used in the icon callback
                n->addChildNode(ImGui::TreeViewNodeData("Ireland"));
                n->addChildNode(ImGui::TreeViewNodeData("Sweden"));
                n->addChildNode(ImGui::TreeViewNodeData("Germany"));
                n->addChildNode(ImGui::TreeViewNodeData("Mexico"));
                n->addChildNode(ImGui::TreeViewNodeData("China"));
                n = tv.addRootNode(ImGui::TreeViewNodeData("SecondRoot Node"));
                n->setUserId(2);    // (optional) used in the icon callback
                n->addChildNode(ImGui::TreeViewNodeData("SecondRoot-FirstChild Node"));
                ImGui::TreeViewNode* n2 = n->addChildNode(ImGui::TreeViewNodeData("SecondRoot-SecondChild Node","Node with a configurable text color"));
                n2->addState(ImGui::TreeViewNode::STATE_COLOR2);    // configurable color set
                n2->setUserId(8);    // (optional) used in the icon callback
                n2->addChildNode(ImGui::TreeViewNodeData("SecondRoot-SecondChild-FirstChild Node"));
                // Test: Now we add the last two root nodes in reverse order:
                n = tv.addRootNode(ImGui::TreeViewNodeData("FourthRoot Node","This is NOT a leaf node\nbut has no child nodes.\n(Good for folder browsers\nif we monitor STATE_OPEN changes)."),-1,true); // Note the last arg: this node can be opened even if has no child nodes (you can add a callback for state changes).
                n->setUserId(3);    // (optional) used in the icon callback
                n = tv.addRootNode(ImGui::TreeViewNodeData("ThirdRoot Node","This is both a root and a leaf node."),2); // put this node at place 2 (0 based).
                n->setUserId(4);    // (optional) used in the icon callback
            }

            // Optional tuff to change some tv options on the fly -------------------------------------
            if (ImGui::TreeNode("Options:##TreeNodeOptions")) {
            {
                bool changed = false;
                ImGui::AlignTextToFramePadding();ImGui::TextDisabled("Selection Mode:");ImGui::SameLine();
                changed|=ImGui::CheckboxFlags("Root##TreeViewSelectRoot",&tv.selectionMode,ImGui::TreeViewNode::MODE_ROOT);ImGui::SameLine();
                changed|=ImGui::CheckboxFlags("Intermediate##TreeViewSelectIntermediate",&tv.selectionMode,ImGui::TreeViewNode::MODE_INTERMEDIATE);ImGui::SameLine();
                changed|=ImGui::CheckboxFlags("Leaf##TreeViewSelectLeaf",&tv.selectionMode,ImGui::TreeViewNode::MODE_LEAF);
                if (tv.selectionMode!=ImGui::TreeViewNode::MODE_NONE) {
                    ImGui::SameLine();
                    changed|=ImGui::Checkbox("MultiSelect##TreeViewMultiSelect",&tv.allowMultipleSelection);
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","using CTRL + LMB");
                }
                if (changed) tv.removeStateFromAllDescendants(ImGui::TreeViewNode::STATE_SELECTED);

                changed = false;
                ImGui::AlignTextToFramePadding();ImGui::TextDisabled("Checkbox Mode:");ImGui::SameLine();
                changed|=ImGui::CheckboxFlags("Root##TreeViewCheckboxRoot",&tv.checkboxMode,ImGui::TreeViewNode::MODE_ROOT);ImGui::SameLine();
                changed|=ImGui::CheckboxFlags("Intermediate##TreeViewCheckboxIntermediate",&tv.checkboxMode,ImGui::TreeViewNode::MODE_INTERMEDIATE);ImGui::SameLine();
                changed|=ImGui::CheckboxFlags("Leaf##TreeViewCheckboxLeaf",&tv.checkboxMode,ImGui::TreeViewNode::MODE_LEAF);
                if (tv.checkboxMode!=ImGui::TreeViewNode::MODE_NONE) {
                    ImGui::SameLine();
                    changed|=ImGui::Checkbox("Automatic##TreeViewAutoCheckChildNodes",&tv.allowAutoCheckboxBehaviour);
                    if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","auto parent/child checks");
                }
                if (changed) tv.removeStateFromAllDescendants(ImGui::TreeViewNode::STATE_CHECKED);
            }
            bool hasDrawIconCb = tv.getTreeViewNodeDrawIconCb();
            if (ImGui::Checkbox("Show icons",&hasDrawIconCb)) {
                tv.setTreeViewNodeDrawIconCb(hasDrawIconCb ? &MyTreeViewNodeDrawIconCallback : NULL);
            }
            ImGui::SameLine();
            ImGui::Checkbox("Inherit Disabled Look##TreeViewInheritDisabledLook",&tv.inheritDisabledLook);
            // Test: static methods for using custom glyphs
            bool customArrow = ImGui::TreeView::HasCustomArrowGlyphs();
            if (ImGui::Checkbox("Use custom arrows##TreeViewCustomArrows",&customArrow)) ImGui::TreeView::SetFontArrowGlyphs(customArrow?">":"",customArrow?"v":"");
            ImGui::SameLine();
            bool customCheckBox = ImGui::TreeView::HasCustomCheckBoxGlyphs();
            if (ImGui::Checkbox("Use custom check boxes##TreeViewCustomCheckBoxes",&customCheckBox)) ImGui::TreeView::SetFontCheckBoxGlyphs(customCheckBox?"[ ]":"",customCheckBox?"[x]":"");
#           if (!defined(NO_IMGUIHELPER) && !defined(NO_IMGUIHELPER_SERIALIZATION))
            //ImGui::Separator();
            static const char* saveName = "myTreeView.layout";
            const char* saveNamePersistent = "/persistent_folder/myTreeView.layout";
            const char* pSaveName = saveName;
#           ifndef NO_IMGUIHELPER_SERIALIZATION_SAVE
            if (ImGui::Button("Save TreeView")) {
#               ifdef YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                pSaveName = saveNamePersistent;
#               endif //YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                if (tv.save(pSaveName))   {
#                   ifdef YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                    ImGui::EmscriptenFileSystemHelper::Sync();
#                   endif //YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                }
            }
            ImGui::SameLine();
#           endif //NO_IMGUIHELPER_SERIALIZATION_SAVE
#           ifndef NO_IMGUIHELPER_SERIALIZATION_LOAD
            if (ImGui::Button("Load TreeView")) {
#               ifdef YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                if (ImGuiHelper::FileExists(saveNamePersistent)) pSaveName = saveNamePersistent;
#               endif //YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                tv.load(pSaveName);
            }
            ImGui::SameLine();
#           endif //NO_IMGUIHELPER_SERIALIZATION_LOAD
#           endif //NO_IMGUIHELPER_SERIALIZATION
            resetTv = ImGui::Button("Reset TreeView");
            // ----------------------------------------------------------------------------------------------
            ImGui::TreePop();
            }
            ImGui::Separator();


            if (tv.render())    // Mandatory call. Makes tv.isInit() return true. Returns true when a node event has occurred.
            {
                // ImGui::TreeViewEvent& event = tv.getLastEvent();
                // event.node       // the ImGui::TreeViewNode* that's changed somehow (can be any node in the hierarchy)
                // event.type       // ImGui::TreeViewNode::EVENT_DOUBLE_CLICKED     // node has been double clicked
                                    // ImGui::TreeViewNode::EVENT_RENAMED              // successfully renamed
                                    // ImGui::TreeViewNode::EVENT_STATE_CHANGED            // the node state has changed and the other fields are valid
                // event.state      // the ImGui::TreeViewNode::STATE_ that's changed (only STATE_OPENED,STATE_SELECTED and STATE_CHECKED are monitored and the latter only when directly clicked by the mouse)
                // event.wasStateRemoved // each state can be set or removed.

                // event.reset();  // optional after processing... (don't call it, why call it?)
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Timeline (https://github.com/nem0/LumixEngine/blob/timeline_gui/external/imgui/imgui_user.inl):");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Lumix Engine's Timeline\nUse CTRL+MW to zoom\nUse CTRL+RMB drag to pan\nUse CTRL+MMB to reset pan and zoom");
        ImGui::Separator();
        static ImVec2 pan_and_zoom(0.f,1.f);	// optional (CTRL+RMB drag: pan in [0.f,1.f]; CTRL+MW: zoom in [0.25f,4.f]). Thanks @meshula!
        if (ImGui::BeginTimeline("MyTimeline",50.f,4,6,&pan_and_zoom))  // label, max_value, num_visible_rows, opt_exact_num_rows (for item culling)
        {
            static float events[12]={10.f,20.f,0.5f,30.f,40.f,50.f,20.f,40.f,15.f,22.5f,35.f,45.f};
            if (ImGui::TimelineEvent("Event1",&events[0])) {/*events[0] and/or events[1] modified*/}
            ImGui::TimelineEvent("Event2",&events[2]);
            ImGui::TimelineEvent("Event3",&events[4],true);    // Event3 can only be shifted
            ImGui::TimelineEvent("Event4",&events[6]);
            ImGui::TimelineEvent("Event5",&events[8]);
            ImGui::TimelineEvent("Event6",&events[10]);
        }
        static float timeline_elapsed_time = 0.f;
        ImGui::EndTimeline(5,timeline_elapsed_time);  // num_vertical_grid_lines, current_time (optional), timeline_running_color (optional)

        // COMPLETELY Optional (And Manual): Timeline Start/Pause Buttons:-------
        static ImU32 timeline_state = 0;    // Manually handled by us (0 = stopped, 1 = playing, 2 = paused)
        static float timeline_begin_time = ImGui::GetTime();
        if (timeline_state==1) {
            // It's playing
            timeline_elapsed_time = ImGui::GetTime()-timeline_begin_time;
            if (timeline_elapsed_time>50.f) {
                // We reset the timer after 50.f seconds here
                timeline_state=0;   // Stopped
                timeline_elapsed_time=0.f;
            }
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY()-ImGui::GetTextLineHeightWithSpacing()); // Go up one line
        if (ImGui::Button(timeline_state==1 ?  " || ###timeline_play" : " > ###timeline_play")) {
            if (timeline_state==1) timeline_state=2;	// Paused
            else if (timeline_state==0)  {
                // Was stopped
                timeline_state=1;   // Now playing
                timeline_begin_time = ImGui::GetTime();	// From zero time
            }
            else {
                // Was paused
                timeline_state=1;   // Now playing
                const float pausedTime = ImGui::GetTime() - timeline_begin_time - timeline_elapsed_time;
                timeline_begin_time+= pausedTime;	// From last time
            }
        }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s",timeline_state==1 ? "Pause" : "Play");
        if (timeline_state!=0)	{
            ImGui::SameLine(0,0);
            if (ImGui::Button(" O ###timeline_stop")) {
                timeline_state=0;   // Stopped
                timeline_elapsed_time=0.f;  // We reset the timer
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Stop");
        }
        // End COMPLETELY Optional (And Manual) Stuff-----------------------------

        // Password Drawer
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Password Drawer Widget:");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Basically it's a widget you can\ndraw a password in, by mouse dragging.");
        ImGui::Separator();
        if (ImGui::TreeNode("Password Drawer##Password Drawer Widget"))    {
            static char password[37] = "";
            static int gridComboSelection = 1;  // We start with option 1 -> 3x3 grid
            static int passwordSize = (2+gridComboSelection)*(2+gridComboSelection)+1;  // +1 -> trailing '\0'
            ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.1f);
            static const char* gridComboItemNames[5]= {"2","3","4","5","6"};
            if (ImGui::Combo("Grid Size##MobileLockGridSize",&gridComboSelection,gridComboItemNames,5))   {
                passwordSize = 2+gridComboSelection;
                passwordSize*=passwordSize;
                passwordSize+=1;  // +1 -> trailing '\0'
                password[0]='\0';   // reset password
            }
            ImGui::PopItemWidth();
            static char passwordDisplayedBelow[37] = "";
            if (ImGui::PasswordDrawer(password,passwordSize))   {
                strcpy(passwordDisplayedBelow,password);
                password[0]='\0';   // reset password
            }
            //ImGui::Text("Live Password: %s",password);// DBG
            if (strlen(passwordDisplayedBelow)>0) ImGui::Text("Password: %s",passwordDisplayedBelow);

            // A commented out example of a read-only version [note that must be: passwordSize = gridSize*gridSize+1, even if password it's shorter]
            //ImGui::Spacing();ImGui::Separator();if (ImGui::PasswordDrawer((char*)"15984",10,ImGuiPasswordDrawerFlags_ReadOnly,250)) {printf("Whole read-only widget clicked\n");fflush(stdout);}
            ImGui::TreePop();
        }



        // Knob
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("KnobFloat(...) from https://github.com/ocornut/imgui/issues/942");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Use Hover+double-click to reset knobs");
        ImGui::Separator();

        static float knobValues[3]={-10.f,0.f,10.f};
        ImGui::KnobFloat("Knob1", &knobValues[0], -10.f, 10.f);ImGui::SameLine();
        ImGui::KnobFloat("Knob2", &knobValues[1], -10.f, 10.f);ImGui::SameLine();
        ImGui::KnobFloat("Knob3", &knobValues[2], -10.f, 10.f);

        // LoadingIndicatorCircle
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("LoadingIndicatorCircle(...) from https://github.com/ocornut/imgui/issues/1901");
        ImGui::Separator();
        ImGui::TextUnformatted("Test 1:");ImGui::SameLine();
        ImGui::LoadingIndicatorCircle("MyLIC1");ImGui::SameLine();
        ImGui::TextUnformatted("Test 2:");ImGui::SameLine();
        ImGui::LoadingIndicatorCircle("MyLIC2",1.f,&ImGui::GetStyle().Colors[ImGuiCol_Header],&ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]);
        ImGui::AlignTextToFramePadding();ImGui::TextUnformatted("Test 3:");ImGui::SameLine();ImGui::LoadingIndicatorCircle("MyLIC3",2.0f);
        ImGui::AlignTextToFramePadding();ImGui::TextUnformatted("Test 4:");ImGui::SameLine();ImGui::LoadingIndicatorCircle("MyLIC4",4.0f,&ImGui::GetStyle().Colors[ImGuiCol_Header],&ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered],12,2.f);
        // No idea why AlignFirstTextHeightToWidgets() does not work...


        // LoadingIndicatorCircle2
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("LoadingIndicatorCircle2(...) from https://github.com/ocornut/imgui/issues/1901");
        ImGui::Separator();
        ImGui::TextUnformatted("Test 1:");ImGui::SameLine();
        ImGui::LoadingIndicatorCircle2("MyLIC21");ImGui::SameLine();
        ImGui::TextUnformatted("Test 2:");ImGui::SameLine();
        ImGui::LoadingIndicatorCircle2("MyLIC22",1.f,1.5f,&ImGui::GetStyle().Colors[ImGuiCol_Header]);
        ImGui::AlignTextToFramePadding();ImGui::TextUnformatted("Test 3:");ImGui::SameLine();ImGui::LoadingIndicatorCircle2("MyLIC23",2.0f);
        ImGui::AlignTextToFramePadding();ImGui::TextUnformatted("Test 4:");ImGui::SameLine();ImGui::LoadingIndicatorCircle2("MyLIC24",4.0f,1.f,&ImGui::GetStyle().Colors[ImGuiCol_Header]);
        // No idea why AlignFirstTextHeightToWidgets() does not work...


        // Pie Menu. based on code posted by @thennequin here:
        // https://gist.github.com/thennequin/64b4b996ec990c6ddc13a48c6a0ba68c
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Pie Menu (by @thennequin: hope we can use it)");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Based on code posted by @thennequin here:\nhttps://gist.github.com/thennequin/64b4b996ec990c6ddc13a48c6a0ba68c");
        ImGui::Separator();
        // trigger logic and output
        const int pieMenuMouseButtonTrigger = ImGuiMouseButton_Right;    // Tweakable
        ImGui::Text("Right-click this text for a Pie Menu!");
        const bool pieMenuTriggered = ImGui::IsItemHovered() && ImGui::IsMouseClicked(pieMenuMouseButtonTrigger);
        if (pieMenuTriggered)   ImGui::OpenPopup("PieMenu");

        // example usage
        static const char* pieSelected = NULL;  // used to display output
        if (ImGui::Pie::BeginPopup("PieMenu",pieMenuMouseButtonTrigger))    {
            pieSelected = "";
            if (ImGui::Pie::MenuItem("Test1")) {pieSelected="Test1";}
            if (ImGui::Pie::MenuItem("Test2")) {pieSelected="Test2";}
            if (ImGui::Pie::MenuItem("Test3")) {pieSelected="Test3";}
            if (ImGui::Pie::BeginMenu("Sub"))    {
                if (ImGui::Pie::BeginMenu("Sub sub\nmenu")) {
                    if (ImGui::Pie::MenuItem("SubSub")) {pieSelected="SubSub";}
                    if (ImGui::Pie::MenuItem("SubSub2")) {pieSelected="SubSub2";}
                    ImGui::Pie::EndMenu();
                }
                if (ImGui::Pie::MenuItem("TestSub")) {pieSelected="TestSub";}
                if (ImGui::Pie::MenuItem("TestSub2")) {pieSelected="TestSub2";}
                ImGui::Pie::EndMenu();
            }
            if (ImGui::Pie::BeginMenu("Sub2"))   {
                if (ImGui::Pie::MenuItem("TestSub")) {pieSelected="TestSub";}
                if (ImGui::Pie::BeginMenu("Sub sub\nmenu"))  {
                    if (ImGui::Pie::MenuItem("SubSub")) {pieSelected="SubSub";}
                    if (ImGui::Pie::MenuItem("SubSub2")) {pieSelected="SubSub2";}
                    ImGui::Pie::EndMenu();
                }
                if (ImGui::Pie::MenuItem("TestSub2")) {pieSelected="TestSub2";}
                ImGui::Pie::EndMenu();
            }
            ImGui::Pie::EndPopup();
        }
        if (pieSelected)    {
            //ImGui::SameLine();
            ImGui::Text("Last selected pie menu item: %s",strlen(pieSelected)==0?"NONE":pieSelected);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Virtual Keyboard");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Based on code posted here: https://gist.github.com/Flix01/78182e9c9e0f9dfad79619e56530568a");
        if (ImGui::TreeNode("virtual keyboard##VK"))    {
            static int keyboardLogicalLayoutIndex = ImGui::KLL_QWERTY;
            static int keyboardPhysicalLayoutIndex = ImGui::KPL_ISO;
            static ImGui::VirtualKeyboardFlags virtualKeyboardFlags = ImGui::VirtualKeyboardFlags_ShowAllBlocks; // ShowAllBlocks displays all the keyboard parts
            static ImGuiKey lastKeyReturned = ImGuiKey_COUNT;

            const float w = ImGui::GetContentRegionAvail().x*0.15f;
            ImGui::SetNextItemWidth(w);
            ImGui::Combo("Logical Layout##VK",&keyboardLogicalLayoutIndex,ImGui::GetKeyboardLogicalLayoutNames(),ImGui::KLL_COUNT);
            ImGui::SameLine(0.f,w);
            ImGui::SetNextItemWidth(w);
            ImGui::Combo("Physical Layout##VK",&keyboardPhysicalLayoutIndex,ImGui::GetKeyboardPhysicalLayoutNames(),ImGui::KPL_COUNT);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x*0.35f);

            static const char* tooltips[6] = {"ShowBaseBlock","ShowFunctionBlock","ShowArrowBlock","ShowKeypadBlock","NoMouseInteraction","NoKeyboardInteraction"};
            int flagHovered = -1;
            if (ImGui::CheckboxFlags("Flags##VKFlags",&virtualKeyboardFlags,6,1,1,0,&flagHovered)) lastKeyReturned = ImGuiKey_COUNT;;
            if (flagHovered>=0 && flagHovered<6) ImGui::SetTooltip("hold SHIFT to toggle the \"%s\" flag",tooltips[flagHovered]);

            ImGui::Spacing();

            ImGuiKey keyReturned =  ImGuiKey_COUNT;

            const ImVec2 childWindowSize(0,350.f*ImGui::GetFontSize()/18.f);
            ImGui::SetNextWindowSize(childWindowSize);
            if (ImGui::BeginChild("VirtualKeyBoardChildWindow##VK",childWindowSize,false,ImGuiWindowFlags_HorizontalScrollbar)) {
                // Draw an arbitrary keyboard layout to visualize translated keys
                keyReturned = ImGui::VirtualKeyboard(virtualKeyboardFlags,(ImGui::KeyboardLogicalLayout) keyboardLogicalLayoutIndex,(ImGui::KeyboardPhysicalLayout) keyboardPhysicalLayoutIndex);
                // ---------------------------------------------------------------
            }
            ImGui::EndChild();
            if (keyReturned!=ImGuiKey_COUNT) lastKeyReturned = keyReturned;
            if (lastKeyReturned!=ImGuiKey_COUNT) ImGui::Text("Last returned ImGuiKey: \"%s\"",lastKeyReturned==ImGuiKey_None?"NONE":ImGui::GetKeyName(lastKeyReturned));

            ImGui::TreePop();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Text Effects");
        if (ImGui::TreeNode("Text Effects"))    {
            // Actually this is not something contained in imguivariuoscontrols... all the code is here!            
            // (Experimental Stuff)

            static double timeStart = ImGui::GetTime();
            double timeNew = ImGui::GetTime();

            static float speed = 5.f;
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x*0.4f);
            ImGui::SliderFloat("Speed##TWTE",&speed,1.f,30.f,"%.0f");
            ImGui::SameLine();
            const bool mustRestart = ImGui::SmallButton("Restart##TWTE");

            bool nodeOpen = ImGui::TreeNode("Typing Effect");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Based on code posted here: https://github.com/ocornut/imgui/issues/5091");
            if (nodeOpen)   {
                // Please see https://github.com/ocornut/imgui/issues/5091 for more info
                static const char* sampleText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
                static const size_t sampleTextLen = strlen(sampleText);

                static double lastTime = timeStart;
                static size_t lastTextOffset = 0;
                static double remainder = 0.f;
                double timePassed = timeNew - lastTime;
                if (mustRestart || timePassed<0.)    {
                    timeStart = lastTime = ImGui::GetTime();
                    lastTextOffset = 0;
                    remainder = 0;
                }

                lastTime = timeNew;
                const double deltaOffsetDouble = (timePassed*speed)+remainder;
                size_t deltaTextOffset = (size_t) deltaOffsetDouble;
                remainder = deltaOffsetDouble-(double)deltaTextOffset;
                lastTextOffset+=deltaTextOffset;
                if (lastTextOffset>sampleTextLen) lastTextOffset=sampleTextLen;

                //ImVec2 p = ImGui::GetCursorPos();

                //ImGui::TextUnformatted(sampleText, sampleText+textOffset);
                ImGui::TextWrapped("%.*s", (int)(lastTextOffset), sampleText);
                //ImGui::Text("%.*s", (int)(textOffset), sampleText);

                //ImGui::SetCursorPos(p);
                //ImGui::Dummy(ImGui::CalcTextSize(sampleText));  // No way, we'd need a ImGui::CalcTextWrappedSize(...) here to reserve the total space in advance...
                // (but maybe we can manually add some '\n' to 'sampleText' and use ImGui::Text("%.*s", (int)(textOffset), sampleText) above to make it work).
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Scrolling Effect"))    {
                // We have prefixed 'sampleText' with a blank space here
                static const char* sampleText = "                                                                                                    "  \
                                                "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
                static const size_t sampleTextLen = strlen(sampleText);

                static double lastTime = timeStart;
                static double remainder = 0.f;
                static const char* startText = sampleText;
                const char* endText=NULL;
                double timePassed = timeNew - lastTime;
                if (mustRestart || timePassed<0.)    {
                    lastTime = timeStart = timeNew;
                    remainder = 0;
                    startText = sampleText;
                    endText=NULL;
                }

                lastTime = timeNew;
                const double deltaOffsetDouble = (timePassed*speed)*8.f+remainder;

                const ImFont* font = ImGui::GetFont();IM_ASSERT(font);
                const float fontSize = ImGui::GetFontSize();
                ImVec2 additionalTextSize = font->CalcTextSizeA(fontSize, deltaOffsetDouble, FLT_MAX, startText, NULL,&endText);
                remainder = deltaOffsetDouble - additionalTextSize.x;
                IM_ASSERT(remainder>=0.f);
                startText = endText;
                if (startText-sampleText>=(long)sampleTextLen)    {
                    lastTime = timeStart = timeNew;
                    remainder = 0;
                    startText = sampleText;
                    endText=NULL;
                }


                const size_t contentRegionAvailX = ImGui::GetContentRegionAvail().x;
                const size_t textHeight = additionalTextSize.y;

                ImDrawList* draw_list = ImGui::GetWindowDrawList();


                ImVec2 p = ImGui::GetCursorScreenPos();

                const float startX = p.x - remainder;   // The second term should make the scrolling smoother (try setting it to zero and see)
                const float endX = p.x+contentRegionAvailX;


                draw_list->PushClipRect(p, ImVec2(endX,p.y+textHeight), true);
                const ImU32 color = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]);
                endText = NULL;
                font->CalcTextSizeA(fontSize, endX, FLT_MAX, startText, NULL,&endText); // Optional line (don't know if it's faster or slower)
                draw_list->AddText(
                            ImVec2(startX,p.y),
                            color,
                            startText,
                            endText
                            );
                draw_list->PopClipRect();

                ImGui::Dummy(ImVec2(contentRegionAvailX,textHeight));

                ImGui::TreePop();
            }            

            nodeOpen = ImGui::TreeNode("Dancing Effect");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Inspired by this post: https://github.com/ocornut/imgui/issues/1286");
            if (nodeOpen)    {
                // Inspired by: https://github.com/ocornut/imgui/issues/1286
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVector<ImDrawVert>&  verts =  draw_list->VtxBuffer;
                const size_t vbstart = verts.size();


                const size_t contentRegionAvailX = ImGui::GetContentRegionAvail().x;
                const size_t textHeight = ImGui::GetFontSize()*2.f;



                ImVec2 p = ImGui::GetCursorScreenPos();

                // Maybe we should clip text with PushClipRect for safety (see the scrolling example)
                const ImU32 color = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]);
                draw_list->AddText(
                            ImVec2(p.x,p.y+textHeight*0.25f),
                            color,
                            "Dancing text!"
                            );

                const size_t vbend = verts.size();IM_ASSERT(vbend>=vbstart);

                ImGui::Dummy(ImVec2(contentRegionAvailX,textHeight));


                //----Start Dancing Effect Code (from 'vbstart' and 'vbend')----


                // Very bad code! But I don't want to include <math.h> for sin
                constexpr float sinTable[7] = {0.f,0.2588f,0.5f,0.701f,0.8660f,0.9659f,1.f}; // [0,90°] stepSize = 15°
                constexpr size_t SinTableSize = sizeof(sinTable)/sizeof(sinTable[0]);
                IM_ASSERT(SinTableSize>1);
                constexpr size_t SinTableSizeMinusOne = sizeof(sinTable)/sizeof(sinTable[0]) - 1;
                constexpr float stepSize = 90.f/SinTableSizeMinusOne;
                constexpr float stepSizeInverse = 1.f/stepSize;
                 IM_ASSERT(stepSize*SinTableSizeMinusOne<=90.01f && stepSize*SinTableSizeMinusOne>=89.99f);

                const float A = textHeight*0.25;
                for (size_t i=0,iSz=vbend-vbstart;i<iSz;i++)  {
                    ImVec2& pos = verts[vbstart+i].pos;

                    float arg = (float)(i/4+1) * timeNew * speed * 5.f; // 4 vertices makes a character
                    while (arg<0.f) arg+=360.f;
                    while (arg>=360.f) arg-=360.f;
                    float sign = 1.0;
                    IM_ASSERT(arg>=0.0 && arg<=360.0);
                    if (arg>180.f)  {
                        sign = -1.0;
                        if (arg<270.f) arg-=180.f;  // to check
                        else arg=360.f-arg;         // to check
                        IM_ASSERT(arg>=0.0 && arg<=90.0);
                    }
                    else if (arg>90.f) arg=180.f-arg;   // sin (90+x) = sin(90-x) -> arg=180.f-arg
                    IM_ASSERT(arg>=0.0 && arg<=90.0);

                    const float argOnStepSize = arg*stepSizeInverse;  // 15.f if the width of the interval in degrees
                    const size_t j = (size_t) argOnStepSize;  // floor
                    IM_ASSERT(j<SinTableSizeMinusOne);
                    const float deltaLow = argOnStepSize - (float)j;  // difference from 'arg' to its lower bound (snapped to 'stepSize') in [0,1]
                    IM_ASSERT(deltaLow>=0.0f && deltaLow<=1.0f);

                    arg = (1.f-deltaLow)*sinTable[j]+deltaLow*sinTable[j+1];    // lerp
                    IM_ASSERT(arg>=0.0 && arg<=1.0);
                    arg*=sign;

                    pos.y += A*arg;
                }

                //----End Dancing Effect Code-----------------------------------

                ImGui::TreePop();
            }

            nodeOpen = ImGui::TreeNode("Rotation Effect");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Inspired by this post: https://github.com/ocornut/imgui/issues/1286");
            if (nodeOpen)    {
                // Inspired by: https://github.com/ocornut/imgui/issues/1286
                static const char* sampleText = "ROTATION";
                const ImVec2 sampleTextSize = ImGui::CalcTextSize(sampleText);
                const ImVec2 bounds(sampleTextSize.x,sampleTextSize.x);


                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVector<ImDrawVert>&  verts =  draw_list->VtxBuffer;
                const size_t vbstart = verts.size();

                const ImVec2 p = ImGui::GetCursorScreenPos();
                const ImVec2 textPos = ImVec2(p.x,p.y+(bounds.y-sampleTextSize.y)*0.5f);

                // Maybe we should clip text with PushClipRect for safety (see the scrolling example)
                const ImU32 color = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]);
                draw_list->AddText(
                            textPos,
                            color,
                            sampleText
                            );

                const size_t vbend = verts.size();IM_ASSERT(vbend>=vbstart);

                ImGui::Dummy(bounds);

                //----Start Rotation Effect Code (from 'textPos', 'vbstart' and 'vbend')----

                const ImVec2 rotationCenter(textPos.x+bounds.x*0.5f,textPos.y+sampleTextSize.y*0.5f);


                static double relativeTime = timeNew;
                static float orientationDegrees = 0.f;
                if (mustRestart) relativeTime = timeNew;
                float elapsed = (timeNew-relativeTime);
                relativeTime = timeNew;

                orientationDegrees+= elapsed * speed * 5.f;


                float sinArg=0.f,cosArg=1.f;

                // Manual calculation of 'sinArg' and 'cosArg' without including <math.h>
                // => It also puts 'orientationDegrees' in the [0°,360°] range
                // If you just replace this code with sin(orientationDegrees*M_PI/180.f) and cos(orientationDegrees*M_PI/180.f), you may want
                // to put 'orientationDegrees' in the  [0°,360°] range too, or just use: orientationDegrees=timeNew*speed*5.f; to simplify code.
                {
                    // Tweakable to improve precision:----------------------------------------
                    //constexpr float sinTable[] = {0.f,0.258819f,0.5f,0.707107f,0.866025f,0.965926f,1.f}; // [0,90°] stepSize = 15°
                    constexpr float sinTable[] = {
                        0.f,0.087156f,0.173648f,0.258819f,0.34202f,0.422618f,0.5f,      // [0°,30°]     stepSize = 5°
                        0.573576f,0.642788f,0.707107f,0.766044f,0.819152f,0.866025f,    // [35°,60°]    stepSize = 5°
                        0.906308f,0.939693f,0.965926f,0.984808f,0.996195f,1.f           // [65°,90°]    stepSize = 5°
                    }; // [0,90°] stepSize = 5°
                    // ------------------------------------------------------------------------

                    constexpr size_t SinTableSize = sizeof(sinTable)/sizeof(sinTable[0]);
                    IM_ASSERT(SinTableSize>1);
                    constexpr size_t SinTableSizeMinusOne = sizeof(sinTable)/sizeof(sinTable[0]) - 1;
                    constexpr float stepSize = 90.f/SinTableSizeMinusOne;
                    constexpr float stepSizeInverse = 1.f/stepSize;
                     IM_ASSERT(stepSize*SinTableSizeMinusOne<=90.01f && stepSize*SinTableSizeMinusOne>=89.99f);
                    // It's: cosTable[a] = sinTable[SinTableSizeMinusOne-a] // for angles in [0,90°]

                    while (orientationDegrees<0.f) orientationDegrees+=360.f;       // to replace fmodf
                    while (orientationDegrees>=360.f) orientationDegrees-=360.f;
                    float arg =orientationDegrees;
                    float signSin = 1.0;float signCos = 1.0;
                    IM_ASSERT(arg>=0.0 && arg<=360.0);
                    if (arg>180.f)  {
                        signSin = -1.0;
                        if (arg<270.f) {
                            arg-=180.f;
                            signCos = -1.0;
                        }
                        else {
                            arg=360.f-arg;
                            signCos = 1.0;
                        }
                        IM_ASSERT(arg>=0.0 && arg<=90.0);
                    }
                    else if (arg>90.f) {
                        arg=180.f-arg;   // sin (90+x) = sin(90-x) -> arg=180.f-arg
                        signCos = -1.0;
                    }
                    IM_ASSERT(arg>=0.0 && arg<=90.0);

                    const float argOnStepSize = arg*stepSizeInverse;  // 15.f if the width of the interval in degrees
                    const size_t j = (size_t) argOnStepSize;  // floor
                    IM_ASSERT(j<SinTableSizeMinusOne);
                    const float deltaLow = argOnStepSize - (float)j;  // difference from 'arg' to its lower bound (snapped to 'stepSize') in [0,1]
                    IM_ASSERT(deltaLow>=0.0f && deltaLow<=1.0f);

                    sinArg = (1.f-deltaLow)*sinTable[j]+deltaLow*sinTable[j+1];    // lerp
                    IM_ASSERT(sinArg>=0.0 && sinArg<=1.0);
                    sinArg*=signSin;

                    const float cosTableJ = sinTable[SinTableSizeMinusOne-j];
                    const float cosTableJPlusOne = sinTable[SinTableSizeMinusOne-(j+1)];
                    cosArg = (1.f-deltaLow)*cosTableJ+deltaLow*cosTableJPlusOne;    // lerp
                    IM_ASSERT(cosArg>=0.0 && cosArg<=1.0);
                    cosArg*=signCos;
                }
                // End Manual calculation code

                for (size_t i=0,iSz=vbend-vbstart;i<iSz;i++)  {
                    ImVec2& pos = verts[vbstart+i].pos;                    
                    const ImVec2 offset(pos.x-rotationCenter.x,pos.y-rotationCenter.y);
                    pos = ImVec2(rotationCenter.x+(offset.x*cosArg-offset.y*sinArg),
                                 rotationCenter.y+(offset.x*sinArg+offset.y*cosArg));
                }

                //----End Rotation Effect Code-----------------------------------

                if (ImGui::IsItemHovered()) ImGui::SetTooltip("orientation: %1.2f° (sin:%1.3f,cos:%1.3f)",orientationDegrees,sinArg,cosArg);

                // Drawback: it wastes too much screen space... better try an alternative 'dancing effect' by
                // rotating contiguous characters a bit CW and CCW... left as an exercise.

                // This technique can probably be applied to texture images too
                // (even if, if we use draw_list->AddImageQuad(...) that takes all 4 vertices, we can just rotate them before the call)

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }


        //ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

#       else //NO_IMGUIVARIOUSCONTROLS
            ImGui::Text("%s","Excluded from this build.\n");
#       endif //NO_IMGUIVARIOUSCONTROLS
            ImGui::TreePop();
        }

        // TabLabels Test:
        if (ImGui::TreeNodeEx("imguitabwindow",collapsingHeaderFlags)) {
        ImGui::TreePop();
#       ifndef NO_IMGUITABWINDOW
        // Based on the code by krys-spectralpixel (https://github.com/krys-spectralpixel), posted here: https://github.com/ocornut/imgui/issues/261
        ImGui::Spacing();
        ImGui::Text("TabLabels (based on the code by krys-spectralpixel):");
        static const char* tabNames[] = {"TabLabelStyle","Render","Layers","Scene","World","Object","Constraints","Modifiers","Data","Material","Texture","Particle"};
        static const int numTabs = sizeof(tabNames)/sizeof(tabNames[0]);
        static const char* tabTooltips[numTabs] = {"Edit the style of these labels","Render Tab Tooltip","This tab cannot be closed","Scene Tab Tooltip","","Object Tab Tooltip","","","","","Tired to add tooltips..."};
        static int tabItemOrdering[numTabs] = {0,1,2,3,4,5,6,7,8,9,10,11};
        static int selectedTab = 0;
        static int optionalHoveredTab = 0;
        static bool allowTabLabelDragAndDrop=true;static bool tabLabelWrapMode = true;static bool allowClosingTabs = false;
        int justClosedTabIndex=-1,justClosedTabIndexInsideTabItemOrdering = -1,oldSelectedTab = selectedTab;

        ImGui::Checkbox("Wrap Mode##TabLabelWrapMode",&tabLabelWrapMode);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","WrapMode is only available\nin horizontal TabLabels");
        ImGui::SameLine();ImGui::Checkbox("Drag And Drop##TabLabelDragAndDrop",&allowTabLabelDragAndDrop);
        ImGui::SameLine();ImGui::Checkbox("Closable##TabLabelClosing",&allowClosingTabs);ImGui::SameLine();
        bool resetTabLabels = ImGui::SmallButton("Reset Tabs");if (resetTabLabels) {selectedTab=0;for (int i=0;i<numTabs;i++) tabItemOrdering[i] = i;}

        /*const bool tabSelectedChanged =*/ ImGui::TabLabels(numTabs,tabNames,selectedTab,tabTooltips,tabLabelWrapMode,&optionalHoveredTab,&tabItemOrdering[0],allowTabLabelDragAndDrop,allowClosingTabs,&justClosedTabIndex,&justClosedTabIndexInsideTabItemOrdering);
        // Optional stuff
        if (justClosedTabIndex==1) {
            tabItemOrdering[justClosedTabIndexInsideTabItemOrdering] = justClosedTabIndex;   // Prevent the user from closing Tab "Layers"
            selectedTab = oldSelectedTab;   // This is safer, in case we had closed the selected tab
        }
        //if (optionalHoveredTab>=0) ImGui::Text("Mouse is hovering Tab Label: \"%s\".\n\n",tabNames[optionalHoveredTab]);

        // Draw tab page
        ImGui::BeginChild("MyTabLabelsChild",ImVec2(0,150),true);
        ImGui::Text("Tab Page For Tab: \"%s\" here.",selectedTab>=0?tabNames[selectedTab]:"None!");
        if (selectedTab==0) {
            static bool editTheme = false;
            ImGui::Spacing();
            ImGui::Checkbox("Edit tab label style",&editTheme);
            ImGui::Spacing();
            if (editTheme) ImGui::TabLabelStyle::Edit(ImGui::TabLabelStyle().Get());   // This is good if we want to edit the tab label style
            else {
                static int selectedIndex=0;
                ImGui::PushItemWidth(135);
                ImGui::SelectTabLabelStyleCombo("select tab label style",&selectedIndex); // Good for just selecting it
                ImGui::PopItemWidth();
            }
        }
        ImGui::EndChild();

        // ImGui::TabLabelsVertical() are similiar to ImGui::TabLabels(), but they do not support WrapMode.
        // ImGui::TabLabelsVertical() example usage
        static bool verticalTabLabelsAtLeft = true;ImGui::Checkbox("Vertical Tab Labels at the left side##VerticalTabLabelPosition",&verticalTabLabelsAtLeft);
        static const char* verticalTabNames[] = {"Layers","Scene","World"};
        static const int numVerticalTabs = sizeof(verticalTabNames)/sizeof(verticalTabNames[0]);
        static const char* verticalTabTooltips[numVerticalTabs] = {"Layers Tab Tooltip","Scene Tab Tooltip","World Tab Tooltip"};
        static int verticalTabItemOrdering[numVerticalTabs] = {0,1,2};
        static int selectedVerticalTab = 0;
        static int optionalHoveredVerticalTab = 0;
        if (resetTabLabels) {selectedVerticalTab=0;for (int i=0;i<numVerticalTabs;i++) verticalTabItemOrdering[i] = i;}

        const float verticalTabsWidth = ImGui::CalcVerticalTabLabelsWidth();
        if (verticalTabLabelsAtLeft)	{
            /*const bool verticalTabSelectedChanged =*/ ImGui::TabLabelsVertical(verticalTabLabelsAtLeft,numVerticalTabs,verticalTabNames,selectedVerticalTab,verticalTabTooltips,&optionalHoveredVerticalTab,&verticalTabItemOrdering[0],allowTabLabelDragAndDrop,allowClosingTabs,NULL,NULL);
            //if (optionalHoveredVerticalTab>=0) ImGui::Text("Mouse is hovering Tab Label: \"%s\".\n\n",verticalTabNames[optionalHoveredVerticalTab]);
            ImGui::SameLine(0,0);
        }
        // Draw tab page
        ImGui::BeginChild("MyVerticalTabLabelsChild",ImVec2(ImGui::GetWindowWidth()-verticalTabsWidth-2.f*ImGui::GetStyle().WindowPadding.x-ImGui::GetStyle().ScrollbarSize,150),true);
        ImGui::Text("Tab Page For Tab: \"%s\" here.",selectedVerticalTab>=0?verticalTabNames[selectedVerticalTab]:"None!");
        ImGui::EndChild();
        if (!verticalTabLabelsAtLeft)	{
            ImGui::SameLine(0,0);
            /*const bool verticalTabSelectedChanged =*/ ImGui::TabLabelsVertical(verticalTabLabelsAtLeft,numVerticalTabs,verticalTabNames,selectedVerticalTab,verticalTabTooltips,&optionalHoveredVerticalTab,&verticalTabItemOrdering[0],allowTabLabelDragAndDrop,allowClosingTabs,NULL,NULL);
            //if (optionalHoveredVerticalTab>=0) ImGui::Text("Mouse is hovering Tab Label: \"%s\".\n\n",verticalTabNames[optionalHoveredVerticalTab]);
        }

#       else //NO_IMGUITABWINDOW
        ImGui::Text("%s","Excluded from this build.\n");
#       endif //NO_IMGUITABWINDOW
    }

        // BadCodeEditor Test:
    if (ImGui::TreeNodeEx("imguicodeeditor",collapsingHeaderFlags)) {
        ImGui::TreePop();
#       ifndef NO_IMGUICODEEDITOR
        ImGui::Text("ImGui::InputTextWithSyntaxHighlighting(...) [Experimental] (CTRL+MW: zoom):");
        ImGui::TextDisabled("[New] Experimental dynamic string support (see code)");
        ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.35f);
        static int languageIndex = (int) ImGuiCe::LANG_CPP;
        ImGui::Combo("Language##BCE_LanguageCombo",&languageIndex,ImGuiCe::GetLanguageNames(),(int)ImGuiCe::LANG_COUNT);
        ImGui::PopItemWidth();

        static const char* myCode="# include <sadd.h>\n\nusing namespace std;\n\n//This is a comment\nclass MyClass\n{\npublic:\nMyClass() {}\nvoid Init(int num)\n{  // for loop\nfor (int t=0;t<20;t++)\n	{\n     mNum=t; /* setting var */\n     const float myFloat = 1.25f;\n      break;\n	}\n}\n\nprivate:\nint mNum;\n};\n\nstatic const char* SomeStrings[] = {\"One\"/*Comment One*//*Comment*/,\"Two /*Fake Comment*/\",\"Three\\\"Four\"};\n\nwhile (i<25 && i>=0)   {\n\ti--;\nbreak;} /*comment*/{/*This should not fold*/}/*comment2*/for (int i=0;i<20;i++)    {\n\t\t\tcontinue;//OK\n} // end second folding\n\nfor (int j=0;j<200;j++)  {\ncontinue;}\n\n//region Custom Region Here\n{\n//something inside here\n}\n//endregion\n\n/*\nMultiline\nComment\nHere\n*/\n\n/*\nSome Unicode Characters here:\n€€€€\n*/\n\n";
        /*
        // This works with a static buffer (fixed-size)
        static char bceBuffer[1024]="";
        if (bceBuffer[0]=='\0') strcpy(bceBuffer,myCode);   //Bad init (use initGL() to fill the buffer)
        ImGui::InputTextWithSyntaxHighlighting("ITWSH_JustForID",bceBuffer,sizeof(bceBuffer),(ImGuiCe::Language)languageIndex,ImVec2(0,300));
        */

        // This "works"(?) with a ImString (dynamic-size)
        // Tip: ImString is std::string when IMGUISTRING_STL_FALLBACK is defined globally (or at the top of addons/imguistring/imguistring.h)
        static ImString myCodeString = myCode;
        ImGui::InputTextWithSyntaxHighlighting("ITWSHS_JustForID",myCodeString,(ImGuiCe::Language)languageIndex,ImVec2(0,300));

#       else //NO_IMGUICODEEDITOR
        ImGui::Text("%s","Excluded from this build.\n");
#       endif //NO_IMGUICODEEDITOR
        }


#       if (defined(YES_IMGUISTRINGIFIER) && !defined(NO_IMGUIFILESYSTEM) && !defined(NO_IMGUIHELPER)  && !defined(NO_IMGUIHELPER_SERIALIZATION) && !defined(NO_IMGUIHELPER_SERIALIZATION_LOAD))
    if (ImGui::TreeNodeEx("imguistringifier (yes_addon)",collapsingHeaderFlags)) {
        ImGui::TextWrapped("%s","It should allow users to make files embeddable in their source code.");
        ImGui::Separator();
	if (ImGui::TreeNode("Stringify files for embedded usage:")) {
	    typedef struct _SupportedTypes {
                enum Type {
                    TYPE_RAW_BINARY=0
                    ,TYPE_RAW_TEXT
                    ,TYPE_BASE64
                    ,TYPE_BASE85
#                   ifdef IMGUI_USE_ZLIB
                    ,TYPE_GZ
                    ,TYPE_GZBASE64
                    ,TYPE_GZBASE85
#                   endif //IMGUI_USE_ZLIB
#                   ifdef YES_IMGUIBZ2
                    ,TYPE_BZ2
                    ,TYPE_BZ2BASE64
                    ,TYPE_BZ2BASE85
#                   endif //YES_IMGUIBZ2
                    ,NUM_TYPES
                };
                inline static const char** GetNames() {
                    static const char* Names[] = {
                        "Raw (Binary)","Raw (Text-Based)","Base64","Base85"
#                       ifdef IMGUI_USE_ZLIB
                        ,"Gz (Raw)","GzBase64","GzBase85"
#                       endif //IMGUI_USE_ZLIB
#                       ifdef YES_IMGUIBZ2
                        ,"Bz2 (Raw)","Bz2Base64","Bz2Base85"
#                       endif //YES_IMGUIBZ2
                    };
                    IM_ASSERT(sizeof(Names)/sizeof(Names[0])==NUM_TYPES);
                    return &Names[0];
                }
                inline static const char** GetExtensions() {
                    static const char* Names[] = {
                        ".bin.inl",".inl",".b64.inl",".b85.inl"
#                       ifdef IMGUI_USE_ZLIB
                        ,".gz.inl",".gz.b64.inl",".gz.b85.inl"
#                       endif //IMGUI_USE_ZLIB
#                       ifdef YES_IMGUIBZ2
                        ,".bz2.inl",".bz2.b64.inl",".bz2.b85.inl"
#                       endif //YES_IMGUIBZ2
                    };
                    IM_ASSERT(sizeof(Names)/sizeof(Names[0])==NUM_TYPES);
                    return &Names[0];
                }
            } SupportedTypes;

            static bool mustDisplayOkMessage = false;
            static bool useUnsignedByteMode = false;
            static bool noBackslashesAtLineEnds = false;

            // Load dialog
            static ImGuiFs::Dialog loadDlg;
            ImGui::Text("File to stringify:");ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.35f);
            ImGui::InputText("###FiletostringifyloadID",(char*)loadDlg.getChosenPath(),ImGuiFs::MAX_PATH_BYTES,ImGuiInputTextFlags_ReadOnly);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            const bool browseButton = ImGui::Button("...##FiletostringifyloadbuttonID");
            if (browseButton) mustDisplayOkMessage=false;
            loadDlg.chooseFileDialog(browseButton,loadDlg.getLastDirectory());
            const bool filePathToStringifyIsValid = strlen(loadDlg.getChosenPath())>0;

            // Load combo
            static int strType = 0;
            ImGui::BeginGroup();
            ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.25f);
            if (ImGui::Combo("Stringified Type",&strType,SupportedTypes::GetNames(),(int) SupportedTypes::NUM_TYPES,(int)SupportedTypes::NUM_TYPES)) mustDisplayOkMessage = false;
            ImGui::PopItemWidth();
            if (filePathToStringifyIsValid) {
                if (strType==0 || strType==4 || strType==7) ImGui::Checkbox("Serialize UNSIGNED bytes###FiletostringifyunsignedByteChb",&useUnsignedByteMode);
                else if (strType==1)    ImGui::Checkbox("Serialize WITHOUT backslashes at line ends###FiletostringifyNoBackslashesChb",&noBackslashesAtLineEnds);
            }
            ImGui::EndGroup();

            // Action Button
            if (filePathToStringifyIsValid)  {
                ImGui::SameLine();
                if (ImGui::Button("STRINGIFY###FiletostringifygobuttonID")) {
                    mustDisplayOkMessage = false;
                    bool ok = ImGuiFs::FileExists(loadDlg.getChosenPath());
                    ImVector<char> buff;
                    if (ok && ImGuiHelper::GetFileContent(loadDlg.getChosenPath(),buff,true,strType==1 ? "r":"rb") && buff.size()>0)   {
                        ok = false;ImVector<char> buff2;
                        switch (strType)    {
                        case SupportedTypes::TYPE_RAW_BINARY:   ok = ImGui::BinaryStringify(&buff[0],buff.size(),buff2,80,useUnsignedByteMode);break;
                        case SupportedTypes::TYPE_RAW_TEXT:     ok = ImGui::TextStringify(&buff[0],buff2,0,0,noBackslashesAtLineEnds);break;
                        case SupportedTypes::TYPE_BASE64:       ok = ImGui::Base64Encode(&buff[0],buff.size(),buff2,true);break;    // This method has an argument that stringify the output layout for us
                        case SupportedTypes::TYPE_BASE85:       ok = ImGui::Base85Encode(&buff[0],buff.size(),buff2,true);break;    // This method has an argument that stringify the output layout for us
#                       ifdef IMGUI_USE_ZLIB
                        case SupportedTypes::TYPE_GZ:          {
                            ok = ImGui::GzCompressFromMemory(&buff[0],buff.size(),buff2);
                            if (ok) ok = ImGui::BinaryStringify(&buff2[0],buff2.size(),buff,80,useUnsignedByteMode);
                            if (ok) buff.swap(buff2);
                        }
                        break;
                        case SupportedTypes::TYPE_GZBASE64:    ok = ImGui::GzBase64CompressFromMemory(&buff[0],buff.size(),buff2,true);break;
                        case SupportedTypes::TYPE_GZBASE85:    ok = ImGui::GzBase85CompressFromMemory(&buff[0],buff.size(),buff2,true);break;
#                       endif //IMGUI_USE_ZLIB
#                       ifdef YES_IMGUIBZ2
                        case SupportedTypes::TYPE_BZ2:          {
                            ok = ImGui::Bz2CompressFromMemory(&buff[0],buff.size(),buff2);
                            if (ok) ok = ImGui::BinaryStringify(&buff2[0],buff2.size(),buff,80,useUnsignedByteMode);
                            if (ok) buff.swap(buff2);
                        }
                        break;
                        case SupportedTypes::TYPE_BZ2BASE64:    ok = ImGui::Bz2Base64Encode(&buff[0],buff.size(),buff2,true);break;
                        case SupportedTypes::TYPE_BZ2BASE85:    ok = ImGui::Bz2Base85Encode(&buff[0],buff.size(),buff2,true);break;
#                       endif //YES_IMGUIBZ2
                        default: IM_ASSERT(true);break;
                        }

                        if (ok) {
#                           ifndef __EMSCRIPTEN__
                            ImGui::SetClipboardText(&buff2[0]);
#                           else //__EMSCRIPTEN__
                            ImGui::LogToTTY();
                            char filename[ImGuiFs::MAX_FILENAME_BYTES]="";
                            ImGuiFs::PathGetFileName(loadDlg.getChosenPath(),filename);
                            ImGui::LogText("\n\n// \"%s%s\":\n",filename,SupportedTypes::GetExtensions()[strType]);
                            ImGui::LogText("%s",&buff2[0]);
                            ImGui::LogFinish();
#                           endif //__EMSCRIPTEN__
                            mustDisplayOkMessage = true;
                        }
                    }
                }
            }

            if (mustDisplayOkMessage) {
#               ifndef __EMSCRIPTEN__
                ImGui::Text("File stringified successfully and placed in the clipboard.");
#               else // __EMSCRIPTEN__
                ImGui::Text("File stringified successfully and placed in the terminal.");
#               endif //__EMSCRIPTEN__
                ImGui::Text("You can copy it inside your code\nor in a file with extension \"%s\".",SupportedTypes::GetExtensions()[strType]);
            }


            //------------------------------------------------
            ImGui::TreePop(); // Mandatory
        }
    ImGui::TreePop();
    }
#       endif //YES_IMGUISTRINGIFIER

#       ifdef YES_IMGUISDF
        // The following check is to ensure ImGui::SdfAddCharsetFromFile(...) can be called (some users don't like to use FILE* in <stdio.h>, and prefer loading stuff from memory only)
#       if (!defined(NO_IMGUISDF_LOAD) || (defined(IMGUIHELPER_H_) && !defined(NO_IMGUIHELPER_SERIALIZATION) && !defined(NO_IMGUIHELPER_SERIALIZATION_LOAD)))
        if (ImGui::TreeNodeEx("imguisdf (yes_addon)",collapsingHeaderFlags)) {
        // Well, we should move the init stuff to InitGL(), clean up textures, etc. (all skipped to avoid multiple preprocessor branches around this file)
        static ImTextureID sdfTexture = 0;
        static ImGui::SdfTextChunk* sdfTextChunk = NULL;
        static char sdfTextBuffer[8192]="\t";
        static ImGui::SdfAnimation* sdfManualAnimation = NULL;  // Entirely optional (might confuse users)
        if (!sdfTextChunk) {
            static bool mustInit=true;
            if (mustInit)   {
                mustInit = false;
                //---------------------
                const char* sdfFontPath = "fonts/Sdf/DejaVuSerifCondensed-Bold.fnt";
                const char* sdfImagePath = "fonts/Sdf/DejaVuSerifCondensed-Bold.png";
                sdfTexture = ImImpl_LoadTexture(sdfImagePath,0,false,false,false);
                ImGui::SdfCharset* charset = ImGui::SdfAddCharsetFromFile(sdfFontPath,sdfTexture,ImGui::SdfCharsetProperties());
                IM_ASSERT(sdfTexture && charset);
                sdfTextChunk = ImGui::SdfAddTextChunk(charset,ImGui::SDF_BT_OUTLINE,ImGui::SdfTextChunkProperties(20,ImGui::SDF_CENTER,ImGui::SDF_MIDDLE,ImVec2(.5f,.5f),ImVec2(.5f,.5f)));
                IM_ASSERT(sdfTextChunk);

                //static const char* sdfSampleText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
                static const char* sdfSampleText = "Lorem ipsum <COLOR=00FF00FF>dolor</COLOR> sit <I>amet</I>, consectetur <B>adipiscing</B> elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n<SCALE=2.0><HALIGN=LEFT>Left<HALIGN=CENTER>Center<HALIGN=RIGHT>Right</SCALE>";
                strcpy(sdfTextBuffer,sdfSampleText);
                ImGui::SdfAddTextWithTags(sdfTextChunk,sdfTextBuffer); // Actually we can append multiple of these calls together

                const bool enableOptionalStuff = true; // TO FIX
                if (enableOptionalStuff) {
                    sdfManualAnimation = ImGui::SdfAddAnimation();
                    ImGui::SdfTextChunkSetManualAnimation(sdfTextChunk,sdfManualAnimation); // This does not activate it by default
                }
            }
        }
        if (sdfTextChunk)   {
            static bool enableSdfTextChunk = false;
            ImGui::Checkbox("Enable SDF Test",&enableSdfTextChunk);
            if (enableSdfTextChunk) {
                if (ImGui::CollapsingHeader("SDF On Screen Text")) ImGui::SdfTextChunkEdit(sdfTextChunk,sdfTextBuffer,sizeof(sdfTextBuffer)/sizeof(sdfTextBuffer[0]));

                // To render all sdf text chunks:
                ImGui::SdfRender(); // This should be moved at the top of DrawGL(), to work when ImGui is not active too. [or after ImGui::Render(), but that spot is not supported at the moment here...]
            }
         }
         ImGui::TreePop();
         }
#       endif // (!defined(NO_IMGUISDF_LOAD) ...)
#       endif //YES_IMGUISDF

#       ifdef YES_IMGUISOLOUD
        if (ImGui::TreeNodeEx("imguisoloud (yes_addon)",collapsingHeaderFlags)) {

        // Code dirty-copied from the SoLoud Welcome example (I'm a newbie...)
        static SoLoud::Soloud soloud; // Engine core

        static bool soloudEnabled = false;
        if (soloudEnabled)  {
            if (ImGui::Button("Stop SoLoud Test")) {
                soloudEnabled=false;
                //soloud.stopAll(); // For some odd reasons, if I stop all, then I cannot restart piano (tested WITH_OPENAL only)
                soloud.deinit();
            }
        }
        else if (ImGui::Button("Start SoLoud Test")) {
            soloudEnabled = true;
            soloud.init();// Initialize SoLoud (the opposite should be: soloud.stopAll();soloud.deinit();)
            //soloud.setVisualizationEnable(1); // enable visualization for FFT calc

            // START SOME DEMO SOUNDS HERE ==================================
            {
#           ifndef NO_IMGUISOLOUD_WAV   // Wav audiosource (SoLoud::WavStream and SoLoud::Wav (i.e. .wav,.ogg) is enabled by default. All the rest must be manually enabled a.t.p.l.)
                // Here we play some files soon after initing SoLoud:
                static SoLoud::WavStream ogg; // (SoLoud::Wav still works with ogg too)
                ogg.load("tetsno.ogg");       // Load an ogg file in streaming mode
                soloud.play(ogg);

                static SoLoud::Wav wav;                 // One sample source
                wav.load("AKWF_c604_0024.wav");       // Load a wave file
                //wav.setLooping(1);                          // Tell SoLoud to loop the sound
                /*int handle1 = */soloud.play(wav);             // Play it
                //soloud.setVolume(handle1, 0.5f);            // Set volume; 1.0f is "normal"
                //soloud.setPan(handle1, -0.2f);              // Set pan; -1 is left, 1 is right
                //soloud.setRelativePlaySpeed(handle1, 0.9f); // Play a bit slower; 1.0f is normal
#           endif //NO_IMGUISOLOUD_WAV

            // These should work too (AFAIK), but audio files are missing...
            /*
#           ifdef YES_IMGUISOLOUD_MODPLUG
                static SoLoud::Modplug mod;
                mod.load("audio/BRUCE.S3M");
                soloud.play(mod);
#           endif //YES_IMGUISOLOUD_MODPLUG
#           ifdef YES_IMGUISOLOUD_MONOTONE
                static SoLoud::Monotone mon;
                mon.load("audio/Jakim - Aboriginal Derivatives.mon");
                soloud.play(mon);
#           endif //YES_IMGUISOLOUD_MONOTONE
#           ifdef YES_IMGUISOLOUD_TEDSID
                static SoLoud::TedSid ted;
                ted.load("audio/ted_storm.prg.dump");
                soloud.play(ted);
                static SoLoud::TedSid sid;
                sid.load("audio/Modulation.sid.dump");
                soloud.play(sid);
#           endif //YES_IMGUISOLOUD_TEDSID
            */
            // (soloud.getVoiceCount()==0) can be used to check if any sound is playing
            }
            // ==============================================================
        }

        if (soloudEnabled && ImGui::TreeNode("SoLoud Stuff"))   {
            // extra sources need additional definitions (at the project level not here), unless you define YES_IMGUISOLOUD_ALL (a.t.p.l. not here).
            // For example:
#           ifdef YES_IMGUISOLOUD_SFXR
            static const char* presets[]= {"NONE","COIN","LASER","EXPLOSION","POWERUP","HURT","JUMP","BLIP"};
            static int chosen=0;
            ImGui::PushItemWidth(ImGui::GetWindowWidth()/3);
            bool sfxOk = ImGui::Combo("Sound Effext##SoLoudSundEffect",&chosen,&presets[0],sizeof presets/sizeof presets[0],sizeof presets/sizeof presets[0]);
            ImGui::PopItemWidth();
            if (chosen>0)   {
                ImGui::SameLine();
                sfxOk|=ImGui::Button("play again##SoLoudSundEffectPlay");
            }
            if (sfxOk && chosen>0)   {
                static SoLoud::Sfxr sfxr;
                sfxr.loadPreset(chosen-1,0);
                soloud.play(sfxr);
            }
#           endif //YES_IMGUISOLOUD_SFXR
#           ifdef YES_IMGUISOLOUD_SPEECH
            static char buf[512]={0};
            if (buf[0]==0) strcpy(&buf[0],"Hello world!");
            ImGui::PushItemWidth(ImGui::GetWindowWidth()/2);
            bool ttsOk = ImGui::InputText("Text to speech##SoLoudTTS",buf,sizeof(buf),ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ttsOk|=ImGui::Button("speak##SoLoudSundEffectSpeak");
            if (ttsOk) {
                static SoLoud::Speech speech; // A sound source (speech, in this case)
                speech.setText(buf);
                speech.setVolume(7);
                soloud.play(speech);    // Play the sound source (we could do this several times if we wanted)
            }
#           endif //YES_IMGUISOLOUD_SPEECH
#           if  (!defined(NO_IMGUIFILESYSTEM) && !defined(NO_IMGUIHELPER)  && !defined(NO_IMGUIHELPER_SERIALIZATION) && !defined(NO_IMGUIHELPER_SERIALIZATION_LOAD))
#           ifdef YES_IMGUISOLOUD_MODPLUG
            {
                // Load dialog
                static ImGuiFs::Dialog loadDlg;
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Mod File:");ImGui::SameLine();
                ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.35f);
                ImGui::InputText("###ModFiletoplayloadID",(char*)loadDlg.getChosenPath(),ImGuiFs::MAX_PATH_BYTES,ImGuiInputTextFlags_ReadOnly);
                ImGui::PopItemWidth();
                ImGui::SameLine(0,0);
                const bool browseButton = ImGui::Button("...##ModFiletoplayloadbuttonID");
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("Supported extensions:\n\"%s\"\n(but .mid and .abc files require additional setup to work).",SoLoud::Modplug::SupportedExtensions);
                loadDlg.chooseFileDialog(browseButton,loadDlg.getLastDirectory(),SoLoud::Modplug::SupportedExtensions);
                const bool filePathToStringifyIsValid = strlen(loadDlg.getChosenPath())>0;
                ImGui::SameLine(0.f,50.f);
                static SoLoud::Modplug mod;
                if (ImGui::Button("Play##ModPlayButton")) {
                    if (filePathToStringifyIsValid && ImGuiFs::FileExists(loadDlg.getChosenPath()))   {
                        soloud.stopAudioSource(mod);
                        mod.load(loadDlg.getChosenPath());
                        soloud.play(mod);
                    }
                }
                ImGui::SameLine(0,0);
                if (ImGui::Button("Stop##ModStopButton")) {
                    soloud.stopAudioSource(mod);
                }
            }
#           endif //YES_IMGUISOLOUD_MODPLUG
#           endif //!defined(NO_IMGUIFILESYSTEM)
#           ifdef IMGUISOLOUD_HAS_BASICPIANO    // This is an automatic definition (that depends on other definitions)
            const bool pianoEnabled = ImGui::TreeNode("Play piano using the PC keyboard");
            if (ImGui::IsItemHovered()) {
                if (pianoEnabled) ImGui::SetTooltip("%s","...listening to PC\nclose this node\nwhen done, please");
                else ImGui::SetTooltip("%s","open this node\nand start playing!");
            }
            if (pianoEnabled)      {
                static ImGuiSoloud::BasicPiano piano;
                if (!piano.isInited()) {
                    piano.init(soloud);
                }
                //-- Optional Choose Keyboard Layout ---
                static int keyboardLayout = ImGuiSoloud::BasicPiano::GetKeyboardLayout();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x*0.4f);
                if (ImGui::Combo("Keyboard Layout##ImGuiSoloud::BasicPiano",&keyboardLayout,ImGuiSoloud::BasicPiano::GetKeyboardLayoutEnumNames(),ImGuiSoloud::BasicPiano::KL_COUNT))   {
                    IM_ASSERT(keyboardLayout>=0 && keyboardLayout<ImGuiSoloud::BasicPiano::KL_COUNT);
                    ImGuiSoloud::BasicPiano::SetKeyboardLayout((ImGuiSoloud::BasicPiano::KeyboardLayout) keyboardLayout);
                }
                //--------------------------------------
                piano.play();
                ImGui::Spacing();
                ImGui::Text("Keys: %s %s %s %s %s %s %s %s %s %s",  // We don't display keys for piano black keys
                            ImGuiSoloud::BasicPiano::GetKeyName(1), // 0 -> F#, 1 -> G and so on...
                            ImGuiSoloud::BasicPiano::GetKeyName(3),
                            ImGuiSoloud::BasicPiano::GetKeyName(5),
                            ImGuiSoloud::BasicPiano::GetKeyName(6),
                            ImGuiSoloud::BasicPiano::GetKeyName(8),
                            ImGuiSoloud::BasicPiano::GetKeyName(10),
                            ImGuiSoloud::BasicPiano::GetKeyName(11),
                            ImGuiSoloud::BasicPiano::GetKeyName(13),
                            ImGuiSoloud::BasicPiano::GetKeyName(15),
                            ImGuiSoloud::BasicPiano::GetKeyName(17)
                            );
                piano.renderGUI();

                ImGui::TreePop();
            }
#           endif //IMGUISOLOUD_HAS_BASICPIANO
            ImGui::TreePop();
        }
        ImGui::TreePop();
        }
#       endif //YES_IMGUISOLOUD

#       ifdef YES_IMGUISQLITE3
        if (ImGui::TreeNodeEx("imguisqlite3 (yes_addon)",collapsingHeaderFlags)) {
        // Just a simple test here (based on http://www.codeproject.com/Articles/6343/CppSQLite-C-Wrapper-for-SQLite)
        static bool testDone = false;
        static bool performSQLiteTest = false;
	if (!testDone) {
	    ImGui::Checkbox("Perform CppSQLite test##SQLiteTest",&performSQLiteTest);
	    if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","It might take some time...");
	}
        static ImGuiTextBuffer rv;
        if (!testDone && performSQLiteTest) {
	    testDone = true;
	    PerformCppSQLiteTest(rv,50000);
        }
	if (testDone && ImGui::TreeNode("SQLite3 Test Result")) {
            ImGui::TextUnformatted(&rv.Buf[0]);
	    ImGui::TreePop();
        }
        ImGui::TreePop();
        }
#       endif //YES_IMGUISQLITE3

        // ListView Test:
    if (ImGui::TreeNodeEx("imguilistview",collapsingHeaderFlags)) {
        ImGui::TreePop();
#       ifndef NO_IMGUILISTVIEW
        MyTestListView();
#       else //NO_IMGUILISTVIEW
        ImGui::Text("%s","Excluded from this build.\n");
#       endif //NO_IMGUILISTVIEW        
    }

    }
    ImGui::End();   // ImGui::Begin("ImGui Addons");


    // 2. Show another simple window, this time using an explicit Begin/End pair
#   ifndef NO_IMGUITOOLBAR
    if (show_another_window)
    {
        //if (ImGui::Begin("Another Window", &show_another_window, ImVec2(500,100),bg_alpha,0))   // Old API
        ImGui::SetNextWindowSize(ImVec2(500,100), ImGuiCond_FirstUseEver);
        if (bg_alpha>=0.f) ImGui::SetNextWindowBgAlpha(bg_alpha);
        if (ImGui::Begin("Another Window",&show_another_window,0))
        {
            // imguitoolbar test (note that it can be used both inside and outside windows (see below)
            ImGui::Separator();ImGui::Text("imguitoolbar");ImGui::Separator();
            static ImGui::Toolbar toolbar;
            void* myImageTextureIdVoitPtr = reinterpret_cast<void*>(myImageTextureId2);
            if (toolbar.getNumButtons()==0)  {
                char tmp[1024];ImVec2 uv0(0,0),uv1(0,0);
                for (int i=0;i<9;i++) {
                    strcpy(tmp,"toolbutton ");
                    sprintf(&tmp[strlen(tmp)],"%d",i+1);
                    uv0 = ImVec2((float)(i%3)/3.f,(float)(i/3)/3.f);
                    uv1 = ImVec2(uv0.x+1.f/3.f,uv0.y+1.f/3.f);

                    toolbar.addButton(ImGui::Toolbutton(tmp,myImageTextureIdVoitPtr,uv0,uv1,ImVec2(16,16)));
                }
                toolbar.addSeparator(16);
                toolbar.addButton(ImGui::Toolbutton("toolbutton 11",myImageTextureIdVoitPtr,uv0,uv1,ImVec2(16,16),true,true,ImVec4(0.8,0.8,1.0,1)));  // Note that separator "eats" one toolbutton index as if it was a real button
                toolbar.addButton(ImGui::Toolbutton("toolbutton 12",myImageTextureIdVoitPtr,uv0,uv1,ImVec2(16,16),true,false,ImVec4(1.0,0.8,0.8,1)));  // Note that separator "eats" one toolbutton index as if it was a real button

                toolbar.setProperties(true,false,false,ImVec2(0.0f,0.f),ImVec2(0.25,1));
            }
            const int pressed = toolbar.render();
            if (pressed>=0) fprintf(stderr,"Toolbar1: pressed:%d\n",pressed);

            // Here we can open a child window if we want to toolbar not to scroll
            ImGui::Spacing();ImGui::Text("imguitoolbar can be used inside windows too.\nThe first series of buttons can be used as a tab control.\nPlease resize the window and see the dynamic layout.\n");
        }
        ImGui::End();
    }
#   endif //NO_IMGUITOOLBAR

    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
#   if (!defined(NO_IMGUISTYLESERIALIZER) && !defined(NO_IMGUISTYLESERIALIZER_SAVE_STYLE))
    if (show_test_window)
    {
        //ImGui::SetNewWindowDefaultPos(ImVec2(650, 20));        // Normally user code doesn't need/want to call this, because positions are saved in .ini file. Here we just want to make the demo initial state a bit more friendly!
        ImGui::ShowDemoWindow(&show_test_window);
    }
#   endif // NO_IMGUISTYLESERIALIZER
#   ifndef NO_IMGUINODEGRAPHEDITOR
    if (show_node_graph_editor_window) {
        //if (ImGui::Begin("Example: Custom Node Graph", &show_node_graph_editor_window,ImVec2(700,600),0.95f,ImGuiWindowFlags_NoScrollbar))    // Old API
        ImGui::SetNextWindowSize(ImVec2(700,600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.95f);
        if (ImGui::Begin("Example: Custom Node Graph",&show_node_graph_editor_window,ImGuiWindowFlags_NoScrollbar))
        {
#           ifndef IMGUINODEGRAPHEDITOR_NOTESTDEMO
            ImGui::TestNodeGraphEditor();   // see its code for further info
#           endif //IMGUINODEGRAPHEDITOR_NOTESTDEMO            
        }
        ImGui::End();
    }
#   endif //NO_IMGUINODEGRAPHEDITOR
#   ifndef NO_IMGUIDOCK
    if (show_dock_window)   {
        //if (ImGui::Begin("imguidock window (= lumix engine's dock system)",&show_dock_window,ImVec2(500, 500),0.95f,ImGuiWindowFlags_NoScrollbar))  // Old API
        ImGui::SetNextWindowSize(ImVec2(700,600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.95f);
        if (ImGui::Begin("imguidock window (= lumix engine's dock system)",&show_dock_window,ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::BeginDockspace();
            static char tmp[128];
            for (int i=0;i<10;i++)  {
                sprintf(tmp,"Dock %d",i);
                if (i==9) ImGui::SetNextDock(ImGuiDockSlot_Bottom);// optional
                if(ImGui::BeginDock(tmp))  {
                    ImGui::Text("Content of dock window %d goes here",i);
                }
                ImGui::EndDock();
            }
//=========== OPTIONAL STUFF ===================================================
            static bool draggingLookOpen = true;    // With this next dock has a close button (but its state is not serializable AFAIK)
            // We're also passing a 'default_size' as initial size of the window once undocked
            if (ImGui::BeginDock("Dragging Look",&draggingLookOpen,0,ImVec2(200,350)))    {
                ImGui::Checkbox("Textured##imguidockDraggingLook",&gImGuiDockReuseTabWindowTextureIfAvailable);
            }
            ImGui::EndDock();
//===========END OPTIONAL STUFF =================================================
//========== OPTIONAL STUFF =====================================================
#           if (!defined(NO_IMGUIHELPER) && !defined(NO_IMGUIHELPER_SERIALIZATION))
            if (ImGui::BeginDock("Load/Save"))  {
                static const char* saveName = "myDock.layout";
                const char* saveNamePersistent = "/persistent_folder/myDock.layout";
                const char* pSaveName = saveName;
#               ifndef NO_IMGUIHELPER_SERIALIZATION_SAVE
                if (ImGui::Button("Save")) {
#                   ifdef YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                    pSaveName = saveNamePersistent;
#                   endif //YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                    if (ImGui::SaveDock(pSaveName))   {
#                       ifdef YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                        ImGui::EmscriptenFileSystemHelper::Sync();
#                       endif //YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                    }
                }
                ImGui::SameLine();
#               endif //NO_IMGUIHELPER_SERIALIZATION_SAVE
#               ifndef NO_IMGUIHELPER_SERIALIZATION_LOAD
                if (ImGui::Button("Load")) {
#                   ifdef YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                    if (ImGuiHelper::FileExists(saveNamePersistent)) pSaveName = saveNamePersistent;
#                   endif //YES_IMGUIEMSCRIPTENPERSISTENTFOLDER
                    ImGui::LoadDock(pSaveName);
                }
                ImGui::SameLine();
#               endif //NO_IMGUIHELPER_SERIALIZATION_LOAD
            }
            ImGui::EndDock();   //Load/Save
#           endif //NO_IMGUIHELPER_SERIALIZATION
//=========== END OPTIONAL STUFF ================================================
            ImGui::EndDockspace();
        }
        ImGui::End();
    }
#   endif //NO_IMGUIDOCK
#   ifndef NO_IMGUITABWINDOW
    if (show_tab_windows)   {
    // Some differences compared to imguidock:
        /*
          1) No floating mode for windows.
          2) ImGui::BeginDockspace()/ImGui::EndDockspace() are replaced by instancing an ImGui::TabWindow + calling render()
          3) ImGui::BeginDock()/ImGui::EndDock() are not present. We must add TabLabels manually to the TabWindow and render them (not shown here) through a global callback of by subclassing the TabLabel class and implementing render().
      That's why using imguidock should be the main choice for everybody (I can't drop imguitabwindow, since it contains ImGui::TabLabels(..) too, and is more suitable for ImGui::PanelManager usage).
        */
        static ImGui::TabWindow tabWindows[2];  // Note: there's more than this: there are methods to save/load all TabWindows together, but we don't use them here. Also we don't use "custom callbacks", "TabLabel modified states" and TabLabel context-menus here to keep things simple.
        static bool showTabWindow[2] = {true,true};

        if (showTabWindow[0])   {
            //if (ImGui::Begin("TabWindow1", &showTabWindow[0], ImVec2(400,600),.95f,ImGuiWindowFlags_NoScrollbar))  // Old API
            ImGui::SetNextWindowSize(ImVec2(400,600), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowBgAlpha(0.95f);
            if (ImGui::Begin("TabWindow1", &showTabWindow[0],ImGuiWindowFlags_NoScrollbar))
            {
                ImGui::TabWindow&  tabWindow = tabWindows[0];
                if (!tabWindow.isInited()) {
                    static const char* tabNames[] = {"Test","Render","Layers","Scene","World","Object","Constraints","Modifiers","Data","Material","Texture","Particle","Physics"};
                    static const int numTabs = sizeof(tabNames)/sizeof(tabNames[0]);
                    static const char* tabTooltips[numTabs] = {"Test Tab Tooltip","Render Tab Tooltip","Layers Tab Tooltip","Scene Tab Tooltip","This tab cannot be dragged around","Object Tab Tooltip","","","","This tab cannot be dragged around","Tired to add tooltips...",""};
                    for (int i=0;i<numTabs;i++) {
                        tabWindow.addTabLabel(tabNames[i],tabTooltips[i],i%3!=0,i%5!=4);
                    }
                }
                tabWindow.render(); // Must be inside a window
            }
            ImGui::End();
        }

        if (showTabWindow[1])   {
            //if (ImGui::Begin("TabWindow2", &showTabWindow[1], ImVec2(400,600),.95f,ImGuiWindowFlags_NoScrollbar))  // Old API
            ImGui::SetNextWindowSize(ImVec2(400,600), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowBgAlpha(0.95f);
            if (ImGui::Begin("TabWindow2", &showTabWindow[1],ImGuiWindowFlags_NoScrollbar))
            {
                ImGui::TabWindow&  tabWindow2 = tabWindows[1];
                tabWindow2.render();
            }
            ImGui::End();
        }

        if (!showTabWindow[0] && !showTabWindow[1]) {
            // reset flags
            showTabWindow[0] = showTabWindow[1] = true;
            show_tab_windows = false;
        }
    }
#   endif //NO_IMGUITABWINDOW
    if (show_performance)   {
        //if (ImGui::Begin("Performance Window",&show_performance,ImVec2(0,0),0.9f,ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoResize))   // Old API
        //ImGui::SetNextWindowSize(ImVec2(0,0), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.9f);
        if (ImGui::Begin("Performance Window",&show_performance,ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("Frame rate %.1f FPS",ImGui::GetIO().Framerate);
            ImGui::Text("Num texture bindings per frame: %d",gImGuiNumTextureBindingsPerFrame);
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s","Consider that we're using\njust a single extra texture\nwith all the icons (numbers).");
        }
        ImGui::End();
    }
#   ifdef YES_IMGUIMINIGAMES
#   ifndef NO_IMGUIMINIGAMES_MINE
    if (show_mine_game) {
        //if (ImGui::Begin("Mine Game",&show_mine_game,ImVec2(400,400),.95f,ImGuiWindowFlags_NoScrollbar))  // Old API
        ImGui::SetNextWindowSize(ImVec2(400,400), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(.95f);
        if (ImGui::Begin("Mine Game",&show_mine_game,ImGuiWindowFlags_NoScrollbar))
        {
            static ImGuiMiniGames::Mine mineGame;
            mineGame.render();
        }
        ImGui::End();
    }
#   endif //NO_IMGUIMINIGAMES_MINE
#   ifndef NO_IMGUIMINIGAMES_SUDOKU
    if (show_sudoku_game) {
        //if (ImGui::Begin("Sudoku Game",&show_sudoku_game,ImVec2(400,400),.95f,ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse))  // Old API
        ImGui::SetNextWindowSize(ImVec2(400,400), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(.95f);
        if (ImGui::Begin("Sudoku Game",&show_sudoku_game,ImGuiWindowFlags_NoScrollbar))
        {
            static ImGuiMiniGames::Sudoku sudokuGame;
            sudokuGame.render();
        }
        ImGui::End();
    }
#   endif //NO_IMGUIMINIGAMES_SUDOKU
#   ifndef NO_IMGUIMINIGAMES_FIFTEEN
    if (show_fifteen_game) {
        //if (ImGui::Begin("Fifteen Game",&show_fifteen_game,ImVec2(400,400),.95f,ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse))  // Old API
        ImGui::SetNextWindowSize(ImVec2(400,400), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(.95f);
        if (ImGui::Begin("Fifteen Game",&show_fifteen_game,ImGuiWindowFlags_NoScrollbar))
        {
            static ImGuiMiniGames::Fifteen fifteenGame;
            fifteenGame.render();
        }
        ImGui::End();
    }
#   endif //NO_IMGUIMINIGAMES_FIFTEEN
#   endif //YES_IMGUIMINIGAMES
#   ifdef YES_IMGUIIMAGEEDITOR
    if (show_image_editor)  {
        //if (ImGui::Begin("Image Editor",&show_image_editor,ImVec2(750,600),0.95f,0))   // Old API
        ImGui::SetNextWindowSize(ImVec2(750,600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(.95f);
        if (ImGui::Begin("Image Editor",&show_image_editor))
        {
            static ImGui::ImageEditor imageEditor;
            if (!imageEditor.isInited()) 	{
                if (!imageEditor.loadFromFile("./blankImage.png")) {
                    //fprintf(stderr,"Loading \"./blankImage.png\" Failed.\n");
                }
            }
            imageEditor.render();
        }
        ImGui::End();
    }
#   endif //YES_IMGUIIMAGEEDITOR

    // imguitoolbar test 2: two global toolbars one at the top and one at the left
#   ifndef NO_IMGUITOOLBAR
    // These two lines are only necessary to accomodate space for the global menu bar we're using:
    const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    const ImVec4 displayPortion = ImVec4(0,gMainMenuBarSize.y,displaySize.x,displaySize.y-gMainMenuBarSize.y);

    {
        static ImGui::Toolbar toolbar("myFirstToolbar##foo");
        if (toolbar.getNumButtons()==0)  {
            char tmp[1024];ImVec2 uv0(0,0),uv1(0,0);
            for (int i=0;i<9;i++) {
                strcpy(tmp,"toolbutton ");
                sprintf(&tmp[strlen(tmp)],"%d",i+1);
                uv0 = ImVec2((float)(i%3)/3.f,(float)(i/3)/3.f);
                uv1 = ImVec2(uv0.x+1.f/3.f,uv0.y+1.f/3.f);

                toolbar.addButton(ImGui::Toolbutton(tmp,reinterpret_cast<void*>(myImageTextureId2),uv0,uv1));
            }
            toolbar.addSeparator(16);
            toolbar.addButton(ImGui::Toolbutton("toolbutton 11",reinterpret_cast<void*>(myImageTextureId2),uv0,uv1,ImVec2(32,32),true,false,ImVec4(0.8,0.8,1.0,1)));  // Note that separator "eats" one toolbutton index as if it was a real button
            toolbar.addButton(ImGui::Toolbutton("toolbutton 12",reinterpret_cast<void*>(myImageTextureId2),uv0,uv1,ImVec2(48,24),true,false,ImVec4(1.0,0.8,0.8,1)));  // Note that separator "eats" one toolbutton index as if it was a real button

            toolbar.setProperties(false,false,true,ImVec2(0.5f,0.f),ImVec2(-1,-1),ImVec4(1,1,1,1),displayPortion);
        }
        const int pressed = toolbar.render();
        if (pressed>=0) {printf("Toolbar1: pressed:%d\n",pressed);fflush(stdout);}
    }
    {
        static ImGui::Toolbar toolbar("myFirstToolbar2##foo");
        if (toolbar.getNumButtons()==0)  {
            char tmp[1024];ImVec2 uv0(0,0),uv1(0,0);
            for (int i=8;i>=0;i--) {
                strcpy(tmp,"toolbutton ");
                sprintf(&tmp[strlen(tmp)],"%d",8-i+1);
                uv0=ImVec2((float)(i%3)/3.f,(float)(i/3)/3.f);
                uv1=ImVec2(uv0.x+1.f/3.f,uv0.y+1.f/3.f);

                toolbar.addButton(ImGui::Toolbutton(tmp,reinterpret_cast<void*>(myImageTextureId2),uv0,uv1,ImVec2(24,48)));
            }
            toolbar.addSeparator(16);
            toolbar.addButton(ImGui::Toolbutton("toolbutton 11",reinterpret_cast<void*>(myImageTextureId2),uv0,uv1,ImVec2(24,32),true,false,ImVec4(0.8,0.8,1.0,1)));  // Note that separator "eats" one toolbutton index as if it was a real button
            toolbar.addButton(ImGui::Toolbutton("toolbutton 12",reinterpret_cast<void*>(myImageTextureId2),uv0,uv1,ImVec2(24,32),true,false,ImVec4(1.0,0.8,0.8,1)));  // Note that separator "eats" one toolbutton index as if it was a real button

            toolbar.setProperties(true,true,false,ImVec2(0.0f,0.0f),ImVec2(0.25f,0.9f),ImVec4(0.85,0.85,1,1),displayPortion);

            //toolbar.setScaling(2.0f,1.1f);
        }
        const int pressed = toolbar.render();
        if (pressed>=0) {printf("Toolbar2: pressed:%d\n",pressed);fflush(stdout);}
    }

#   ifndef NO_IMGUIFILESYSTEM
    // Really tiny visual optimization here:
    ImGuiFs::Dialog::WindowLTRBOffsets.x = 24;   // save some pixels from left-right screen borders
    ImGuiFs::Dialog::WindowLTRBOffsets.y = gMainMenuBarSize.y + 32; // save some pixels from top-bottom screen borders
#   endif //NO_IMGUIFILESYSTEM

#   endif //NO_IMGUITOOLBAR
}





//#   define USE_ADVANCED_SETUP   // in-file definition (see below). For now it just adds custom fonts and different FPS settings (please read below).

#ifndef __EMSCRIPTEN__
#include <locale.h> // setlocale(...), optional, see below
#endif //__EMSCRIPTEN__ // I'd save emscripten from this (but it might be used)

// Application code
#ifndef IMGUI_USE_AUTO_BINDING_WINDOWS  // IMGUI_USE_AUTO_ definitions get defined automatically (e.g. do NOT touch them!)
int main(int argc, char** argv)
#else //IMGUI_USE_AUTO_BINDING_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int iCmdShow)
#endif //IMGUI_USE_AUTO_BINDING_WINDOWS
{

#   ifndef __EMSCRIPTEN__
    // [Optional] Here we just set some fields in the C locale to native (""). The default C locale is ("C"), which is ASCII English AFAIK.
    setlocale(LC_CTYPE, "");        // This might affect the encoding of the imguifilesystem paths (but I'm not sure about it...) [Docs say: selects the character classification category of the C locale]
    //setlocale(LC_COLLATE, "");    // ??? This probably affects the way items are sorted
    setlocale(LC_TIME, "");         // This affects imguidatetime (the language of the names of the months)
#   endif //__EMSCRIPTEN__

/*#   ifdef IMGUI_ENABLE_FREETYPE    // Testing only (to remove)
    //gImGuiDefaultFontBuilderFlags are applied to the whole font atlas
    gImGuiDefaultFontBuilderFlags = ImGuiFreeTypeBuilderFlags_Bold|ImGuiFreeTypeBuilderFlags_Oblique; // ImGuiFreeTypeBuilderFlags set globally
#   endif //IMGUI_ENABLE_FREETYPE*/

#   ifndef USE_ADVANCED_SETUP

    //ImImpl_InitParams::DefaultFontSizeOverrideInPixels = 26.f;   // Fast method to override the size of the default font (13.f)

    // Basic
#   ifndef IMGUI_USE_AUTO_BINDING_WINDOWS  // IMGUI_USE_AUTO_ definitions get defined automatically (e.g. do NOT touch them!)
    ImImpl_Main(NULL,argc,argv);
#   else //IMGUI_USE_AUTO_BINDING_WINDOWS
    ImImpl_WinMain(NULL,hInstance,hPrevInstance,lpCmdLine,iCmdShow);
#   endif //IMGUI_USE_AUTO_BINDING_WINDOWS


#   else //USE_ADVANCED_SETUP
    // Advanced
        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x20AC, 0x20AC,	// €
            0x2122, 0x2122,	// ™
            // these are used by ImGui::VirtualKeyboard(...) if available----
            0x2190, 0x2193, // ←↑→↓ arrows + pageUp/pageDown + backspace fallback
            0x2196, 0x2196, // ↖    home
            0x21B5, 0x21B5, // ↵    enter
            0x21B9, 0x21B9, // ↹    tab
            0x21E4, 0x21E5, // ⇤⇥   tab fallback
            0x21E7, 0x21E7, // ⇧    shift
            0x2212, 0x2212, // −    keypad subtract
            0x2215, 0x2215, // ∗    keypad asterisk
            0x2B05, 0x2B07, // ⬅⬆⬇
            0x2302, 0x2302, // ⌂    home fallback
            0x23CF, 0x23CF, // ⏏    capsLock
            0x23f8, 0x23f8, // ⏸   pause
            0x25AE, 0x25AF, // ▮▯   pause fallback
            // --------------------------------------------------------------
            0x263A, 0x263A, // ☺
            0x266A, 0x266A, // ♪
            // --------------------------------------------------------------
            0x27A1, 0x27A1, // ➡
            0x27F5, 0x27F5, // ⟵    backspace // this is used by ImGui::VirtualKeyboard(...) too
            0
        };
    const float fontSizeInPixels = 18.f;
                                  //-40.f; // If < 0, it's the number of lines that fit the whole screen (but without any kind of vertical spacing)
    ImFontConfig cfg;
#   ifdef IMIMPL_BUILD_SDF
    cfg.OversampleH=cfg.OversampleV=1;    // signed-distance-field fonts work better when these values are equal (default: 3,1 are not equal)
    //ImImpl_SdfShaderSetParams(ImVec4(0.460f,0.365f,0.120f,0.04f));	// (optional) Sets sdf params
#   endif //IMIMPL_BUILD_SDF

/*#   ifdef IMGUI_ENABLE_FREETYPE    // Testing only (to remove)
    cfg.FontBuilderFlags|= ImGuiFreeTypeBuilderFlags_Bold|ImGuiFreeTypeBuilderFlags_Oblique;    // ImGuiFreeTypeBuilderFlags on a font basis
#   endif //IMGUI_ENABLE_FREETYPE*/

    // These lines load an embedded font (with no compression).
    const unsigned char ttfMemory[] =
#   include "./fonts/DejaVuSerifCondensed-Bold.ttf.inl"
//#   include "./fonts/DroidSerif-Bold.ttf.inl"
;   // tip: If you have signed chars (e.g. const char ttfMemory[] = ...) you can still cast ttfMemory as (const unsigned char*) later.

    ImImpl_InitParams gImGuiInitParams(
    -1,-1,NULL,                                                         // optional window width, height, title
    //------------------------------------------------------------------------------------------------------------------------
    NULL,
    //"./fonts/DejaVuSerifCondensed-Bold.ttf",                          // optional custom font from file (main custom font)
    //------------------------------------------------------------------------------------------------------------------------
    //NULL,0,
    (const unsigned char*) ttfMemory,sizeof(ttfMemory)/sizeof(ttfMemory[0]),    // optional custom font from memory (secondary custom font) WARNING (licensing problem): e.g. embedding a GPL font in your code can make your code GPL as well.
    //------------------------------------------------------------------------------------------------------------------------
    fontSizeInPixels,
    &ranges[0],
    &cfg,                                                               // optional ImFontConfig* (useful for merging glyph to the default font, according to ImGui)
    false                                                               // true = addDefaultImGuiFontAsFontZero
    );
    // IMPORTANT: If you need to add more than one TTF file,
    // or you need to load embedded font data encoded with a different ImImpl_InitParams::Compression type,
    // there's a second ctr that takes a ImVector<ImImpl_InitParams::FontData> (see imguibindings.h).
    // For a single compressed font loaded from an extern file, the first constructor should work (the file extension is used to detect the compression type).
    // In all cases, to use compressed/encoded data some additional definitions are necessary (for example: YES_IMGUIBZ2 and/or YES_IMGUISTRINGIFIER and/or IMGUI_USE_ZLIB).

    // Here are some optional tweaking of the desired FPS settings (they can be changed at runtime if necessary, but through some global values defined in imguibindinds.h)
    gImGuiInitParams.gFpsClampInsideImGui = 30.0f;  // Optional Max allowed FPS (!=0, default -1 => unclamped). Useful for editors and to save GPU and CPU power.
    gImGuiInitParams.gFpsDynamicInsideImGui = false; // If true when inside ImGui, the FPS is not constant (at gFpsClampInsideImGui), but goes from a very low minimum value to gFpsClampInsideImGui dynamically. Useful for editors and to save GPU and CPU power.
    gImGuiInitParams.gFpsClampOutsideImGui = 10.f;  // Optional Max allowed FPS (!=0, default -1 => unclamped). Useful for setting a different FPS for your main rendering.

//#   define TEST_IMAGE_GLYPHS      // Experimental (currently it works only with user glyphs from uniformly sized tiles in images (or from a whole image) (good for image icons), but we could extend the code in the future if requested to support font glyphs of different widths)
#   ifdef TEST_IMAGE_GLYPHS
    // 'S','P','F'
    ImImpl_InitParams::CustomFontGlyph::ImageData imageData(512,512,"Tile8x8.png",8,8); // The image we want to use for our glyphs
    gImGuiInitParams.customFontGlyphs.push_back(ImImpl_InitParams::CustomFontGlyph(0,'S',imageData, 9));
    gImGuiInitParams.customFontGlyphs.push_back(ImImpl_InitParams::CustomFontGlyph(0,'P',imageData,10));
    gImGuiInitParams.customFontGlyphs.push_back(ImImpl_InitParams::CustomFontGlyph(0,'F',imageData,11));

    // Numbers from 1 to 9
    ImImpl_InitParams::CustomFontGlyph::ImageData imageData2(128,128,"myNumbersTexture.png",3,3); // The image we want to use for our glyphs
    for (int i=0;i<10;i++)   {
        gImGuiInitParams.customFontGlyphs.push_back(ImImpl_InitParams::CustomFontGlyph(0,(ImWchar)('1'+i),imageData2,i,0.f));   // Here we use a zero advance_x_delta (default is 1.0f)
    }

    // Not sure how to specify an ImWchar using a custom definition (like FontAwesome in main2.cpp)...
#   endif //TEST_IMAGE_GLYPHS




#   ifndef IMGUI_USE_AUTO_BINDING_WINDOWS  // IMGUI_USE_AUTO_ definitions get defined automatically (e.g. do NOT touch them!)
    ImImpl_Main(&gImGuiInitParams,argc,argv);
#   else //IMGUI_USE_AUTO_BINDING_WINDOWS
    ImImpl_WinMain(&gImGuiInitParams,hInstance,hPrevInstance,lpCmdLine,iCmdShow);
#   endif //IMGUI_USE_AUTO_BINDING_WINDOWS

#   endif //USE_ADVANCED_SETUP

	return 0;
}






#ifdef YES_IMGUISQLITE3	// yes_addon
#include <time.h>   // just for time mesuring...
void PerformCppSQLiteTest(ImGuiTextBuffer& rv,int nRowsToCreate) {
    rv.Buf.reserve(2048);
    if (nRowsToCreate<=0) nRowsToCreate = 50000;

    using namespace CppSQLite3;
    const char* dbFileName = "./myTestSQLite3.db";
    try {
	int i,fld,nRows;
	DB db;
    rv.appendf("SQLite Version: %s\n",db.SQLiteVersion());
	remove(dbFileName);     // from stdio.h (I must admit I've never used it: does it delete the file ?)

    rv.appendf("Performed test at http://www.codeproject.com/Articles/6343/CppSQLite-C-Wrapper-for-SQLite.\n\n");

	db.open(dbFileName);    // it opens or creates the db
	db.execDML("create table emp(empno int, empname char(20));");   // Creates emp table with a int (empno) and a char(20) (empname)
    nRows = db.execDML("insert into emp values (7, 'David Beckham');");                 rv.appendf("nRows = %d\n",nRows);
    nRows = db.execDML("update emp set empname = 'Christiano Ronaldo' where empno = 7;");   rv.appendf("%d rows updated\n",nRows);
    nRows = db.execDML("delete from emp where empno = 7;");                                 rv.appendf("%d rows deleted\n",nRows);

	// Transaction Demo [The transaction could just as easily have been rolled back]
	clock_t ckStart,ckEnd;
    rv.appendf("\nTransaction test, creating %d rows please wait...\n",nRowsToCreate);
	ckStart = clock();
	db.execDML("begin transaction;");
	for (i = 0; i < nRowsToCreate; i++) {
	    char buf[128];
	    sprintf(buf, "insert into emp values (%d, 'Empname%06d');", i, i);
	    db.execDML(buf);
	}
	db.execDML("commit transaction;");
	ckEnd = clock();
	// Demonstrate CppSQLite::DB::execScalar()
    rv.appendf("%d rows in emp table in %1.3f seconds (it was fast!)\n",db.execScalar("select count(*) from emp;"),(float)(ckEnd-ckStart)/(float)CLOCKS_PER_SEC);

	// Pre-compiled Statements Demo
	db.execDML("drop table emp;");	// SQLITE_LOCKED[6]: database table is locked.
	db.execDML("create table emp(empno int, empname char(20));");
    rv.appendf("\nTransaction test with pre-compiled statements, creating %d rows please wait...\n",nRowsToCreate);
	ckStart = clock();
	db.execDML("begin transaction;");
	Statement stmt = db.compileStatement("insert into emp values (?, ?);");
	for (i = 0; i < nRowsToCreate; i++) {
	    char buf[16];
	    sprintf(buf, "EmpName%06d", i);
	    stmt.bind(1, i);
	    stmt.bind(2, buf);
	    stmt.execDML();
	    stmt.reset();
	}
	db.execDML("commit transaction;");
	ckEnd = clock();
    rv.appendf("%d rows in emp table in %1.3f seconds (that was even faster!)\n",db.execScalar("select count(*) from emp;"),(float)(ckEnd-ckStart)/(float)CLOCKS_PER_SEC);

	// Re-create emp table with auto-increment field
    rv.appendf("\nAuto increment test\n");
	db.execDML("drop table emp;");
	db.execDML("create table emp(empno integer primary key, empname char(20));");

	for (i = 0; i < 5; i++) {
	    char buf[128];
	    sprintf(buf,"insert into emp (empname) values ('Empname%06d');", i+1);
	    db.execDML(buf);
        rv.appendf(" primary key: %ld\n",(long)db.lastRowId());
	}

	// Query data and also show results of inserts into auto-increment field
    rv.appendf("\nSelect statement test\n");
	Query q = db.execQuery("select * from emp order by 1;");
	for (fld = 0; fld < q.numFields(); fld++)   {
        rv.appendf("%s(%s)|",q.fieldName(fld),q.fieldDeclType(fld));   // It was fieldType(fld)...
	}
    rv.appendf("\n");
	while (!q.eof())    {
        rv.appendf("	%s	|   %s	    |\n",q.fieldValue(0),q.fieldDeclType(1));   // It was fieldType(fld)...
	    q.nextRow();
	}

	// SQLite's printf() functionality. Handles embedded quotes and NULLs
    rv.appendf("\nSQLite sprintf test\n");
	Buffer bufSQL;
	bufSQL.format("insert into emp (empname) values (%Q);", "He's bad");
    rv.appendf("%s\n",(const char*)bufSQL);
	db.execDML(bufSQL);

	bufSQL.format("insert into emp (empname) values (%Q);", NULL);
    rv.appendf("%s\n",(const char*)bufSQL);
	db.execDML(bufSQL);

	// Fetch table at once, and also show how to
	// use CppSQLite::Table::setRow() method
    rv.appendf("\ngetTable() test\n");
	Table t = db.getTable("select * from emp order by 1;");

	for (fld = 0; fld < t.numFields(); fld++)   {
        rv.appendf("%s	|",t.fieldName(fld));
	}
    rv.appendf("\n");
	for (int row = 0; row < t.numRows(); row++) {
	    t.setRow(row);
	    for (int fld = 0; fld < t.numFields(); fld++)   {
        if (!t.fieldIsNull(fld))    rv.appendf("	%s	|",t.fieldValue(fld));
        else rv.appendf("	NULL			|");
	    }
        rv.appendf("\n");
	}

	// Test CppSQLite::Binary by storing/retrieving some binary data, checking
	// it afterwards to make sure it is the same
    rv.appendf("\nBinary data test\n");
	db.execDML("create table bindata(desc char(10), data blob);");

	unsigned char bin[256];
	Binary blob;
	for (i = 0; i < (int)sizeof bin; i++)    bin[i] = i;
	blob.setBinary(bin, sizeof bin);

	bufSQL.format("insert into bindata values ('testing', %Q);",
		      blob.getEncoded());
	db.execDML(bufSQL);
    rv.appendf("Stored binary Length: %ld\n",sizeof bin);

	q = db.execQuery("select data from bindata where desc = 'testing';");
	if (!q.eof())   {
	    blob.setEncoded((unsigned char*)q.fieldValue("data"));
        rv.appendf("Retrieved binary Length: %d\n",blob.getBinaryLength());
	}

	const unsigned char* pbin = blob.getBinary();
	for (i = 0; i < (int) sizeof bin; i++)
	{
	    if (pbin[i] != i)   {
        rv.appendf("Problem: i: ,%d bin[i]: %d\n",i,(int)pbin[i]);
	    }
	}

    rv.appendf("\nEnd of tests\n");
    }
    catch (CppSQLite3::Exception& e)
    {
    rv.appendf("Exception thrown. Code: %d. Message: %s.\n",e.errorCode(),e.errorMessage());
    }
}
#endif //YES_IMGUISQLITE3

