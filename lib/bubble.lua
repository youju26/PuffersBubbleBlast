function newBubble(x, y, orientation)
  local bubble = {}
  bubble.x = x
  bubble.y = y
  bubble.orientation = orientation

  bubble.animation = newAnimation(love.graphics.newImage("assets/FX/explosion.png"), 60, 82, 1)

  function bubble:update(dt)
    self.x = self.x + math.cos(self.orientation) * dt * 100
    self.y = self.y + math.sin(self.orientation) * dt * 100
  end

  function bubble:draw()
    love.graphics.draw(self.animation.spriteSheet, self.animation:current_sprite(), self.x, self.y)
  end

  return bubble
end
