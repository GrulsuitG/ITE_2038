SELECT Trainer.hometown, nickname
FROM Trainer, CatchedPokemon, (SELECT hometown, MAX(level) AS 'large'
                                FROM CatchedPokemon, Trainer
                                WHERE owner_id = Trainer.id
                                GROUP BY hometown) AS HTML
WHERE Trainer.id = owner_id AND
      HTML.hometown = Trainer.hometown AND
      HTML.large = level
ORDER BY Trainer.hometown ASC