{-# OPTIONS_GHC -Wno-unrecognised-pragmas #-}
{-# HLINT ignore "Eta reduce" #-}
module Main where

{-# LANGUAGE NoImplicitPrelude #-}

fib :: Int -> Int
fib 0 = 0
fib 1 = 1
fib n = fib (n - 1) + fib (n - 2)

-- >>> fib 7
-- 13

data Vec2 = Vec2 Int Int

len :: Vec2 -> Int
len (Vec2 x y) = x * x + y * y

-- >>> len (Vec2 3 4)
-- 25

data Vec3 = Vec3 Int Int Int
    deriving Show

ext :: Vec2 -> Int -> Vec3
ext (Vec2 x y) z = Vec3 x y z

-- >>> ext (Vec2 1 2) 3
-- Vec3 1 2 3

data Cons a = Cons a (Cons a) | Nil
    deriving Show

-- >>> Cons 'f' (Cons 'o' (Cons 'o' Nil))
-- Cons 'f' (Cons 'o' (Cons 'o' Nil))

addTwo :: Int -> Int
addTwo x = x + 2

consOfInts = [2, 3, 5]
consOfIntsPlusTwo = map addTwo consOfInts

--- >>> consOfIntsPlusTwo
-- [4,5,7]

main :: IO ()
main = putStrLn "Hello, World!"
