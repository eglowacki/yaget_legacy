-- startup script test

log.trace("auto starting 'boot.lua' script...")

local function initYaget()
    local configPath = "P:/Yaget/Repository/Assets/Default/"
    yaget.vfs:add("xml", configPath, "Disk")
    yaget.vfs:add("swf", configPath .. "UI/", "Disk")
    yaget.vfs:add("lua", configPath .. "Scripts/include/", "Disk")
    yaget.vfs:add("fx", configPath .. "Shaders/", "Disk")
    yaget.vfs:add("mat", configPath .. "Materials/", "Disk")

    configPath = "P:/Yaget/Repository/Assets/Doodle/"
    yaget.vfs:add("lua", configPath .. "Scripts/", "Disk")

    configPath = yaget.appPath() .. "/../Assets/"
    yaget.vfs:add("xml", configPath, "Disk")
    yaget.vfs:add("ui", configPath, "Disk")

    configPath = "Scratch/Options/"
    yaget.vfs:add("options", configPath .. "Scripts/", "Memory")

    yaget.input:loadBindings("KeyBindings")
end

initYaget()
require "viewport"
require "format"

Model = class(function(model, name)
                    model.material = yaget.Material(name)
                    model.geometry = nil
                    model.view = nil
                 end)


function Model:render()
    self.material.geometry = self.geometry.token

    self.material:setInput("generic_true", true)
    self.material:setInput("generic_4", 4)
    --self.material:setInput("generic_missing.tga", "missing.tga")
    self.material:setInput("generic_Vector3(1, 2, 3)", math.Vector3(1, 2, 3))
    self.material:setInput("generic_Vector4(3, 4, 5, 6)", math.Vector4(3, 4, 5, 6))

    mover_test(self)
    self.material:render()
end


yaget.module:load('RendererDX9')
yaget.module:load('Editor')


-- ------------------- file scope globals
local elapsed = 0
local showUI = true

local mainViewport = nil
local toolViewport = nil

local planeGeom = nil
local planeModel = nil

local function actionEvent(name)
    log.trace("Action Triggerd: " .. name)
end

local function actionTestUI(name)
    if showUI == true then
        showUI = false
        log.trace("mainViewport UI is NULL")
        mainViewport.view.ui = ""
    else
        showUI = true
        log.trace("mainViewport UI is 'Window'")
        mainViewport.view.ui = "Window"
    end
end


-- This is main game logic
local function tick(deltaTime)
    elapsed = elapsed + deltaTime
    --log.trace("tick: deltaTime: " .. deltaTime .. ", elapsed: " .. elapsed)

    if mainViewport ~= nil then
        str = "FPS: " .. 1/deltaTime
        --str = format_num(1/deltaTime, 2)
        mainViewport:print(10, 10, math.Vector4(1, 1, 1, 1), str)

        -- here we could gather renderable components
        mainViewport:render(function()
                                planeModel:render()
                            end)
    end

    if toolViewport ~= nil then
        toolViewport:render(nil)
    end
end


mainViewport = Viewport("Main")
mainViewport.view.ui = ""
mainViewport:print(10, 10, math.Vector4(1, 1, 1, 1), "Hello World")
mainViewport.clearColor = math.Vector4(0.3, 0.3, 1, 1)
--showUI = false
mainViewport:show(true)

--[[
toolViewport = Viewport("Tool")
toolViewport.view.ui = ""
toolViewport:print(10, 10, math.Vector4(0, 0, 0, 1), "Hello World")
toolViewport.clearColor = math.Vector4(0, 1, 0, 1)
toolViewport:show(true)


local testUI = action("Test.Toggle.UI")
testUI.connect = actionTestUI
]]


local editor = yaget.Editor()
local pluginList = editor:list("")
for key, value in pairs(pluginList) do
    log.trace("Valid plugin: " .. value)
end

local moverScript = yaget.module:script('Geometry/Mover.lua')


local script = yaget.module:script('Geometry/Generators/GridPlane.lua')
planeModel = Model("GridPlane")
planeModel.geometry = yaget.Geometry(function(data) return generators_gridPlane(data) end,
                                     {"position"})
planeModel.view = mainViewport.view

local editorToggle = action("Editor.Toggle")
editorToggle.connect = function()
                            log.trace("Editor.Toggle action triggerd.")
                            local editor = yaget.Editor()
                            editor:load("AssetWatcher")
                       end

local genPlane = action("Test.Generate.Geometry")
genPlane.connect = function()
                        log.trace("Test.Generate.Geometry action triggerd.")
                        planeModel.geometry:generate()
                       end

local quitAction = action("App.Quit")
quitAction.connect = function()
                        log.trace("App.Quit action triggerd.")
                        mainViewport:close()
                     end

yaget.run(tick)



