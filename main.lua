function love.load()
  background = love.graphics.newImage("assets/environment/background.png")
end

-- function love.update()
--   
-- end

function love.draw()
  local width, height = love.graphics.getDimensions()
  love.graphics.draw(background, 0, 0)
end
