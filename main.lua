height = 256
width = 320
scale = 1

function love.load()
  background = love.graphics.newImage("assets/environment/background.png")
  puffy = love.graphics.newImage("assets/puffy.png")

  puffy_normal = love.graphics.newQuad(0,0,200,200,puffy)
  puffy_bloW = love.graphics.newQuad(200,0,200,200,puffy)
  love.resize(love.graphics.getDimensions())
  is_puffy_blow = false
end

function love.update()
  
end

function love.keypressed(key, scancode, isrepeat)
   if key == "escape" then
      love.event.quit()
   end
   if key == "space" then
      is_puffy_blow = true
   end
end

function love.keyreleased(key, scancode, isrepeat)
   if key == "space" then
      is_puffy_blow = false
   end
end

function love.draw()
  love.graphics.scale(scale)
  love.graphics.draw(background, 0, 0)
  love.graphics.scale(.2)
  if is_puffy_blow then
      love.graphics.draw(puffy, puffy_bloW)
    else
      love.graphics.draw(puffy, puffy_normal)
  end
end

function love.resize(_,h)
  scale = h / height
end
