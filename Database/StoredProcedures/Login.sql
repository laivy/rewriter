USE [AccountDB]

IF OBJECT_ID('Login', 'P') IS NOT NULL
    DROP PROCEDURE [Login]
GO

CREATE PROCEDURE [Login]
    @ID NVARCHAR(16),
    @Password NVARCHAR(16)
AS
BEGIN
    DECLARE @AccountID INT = -1

    SELECT @AccountID = [AccountID]
    FROM [dbo].[Account]
    WHERE [ID] = @ID AND PWDCOMPARE(@Password, [Password]) = 1

    IF @AccountID = -1
        RETURN 1

    IF EXISTS (SELECT 1 FROM [dbo].[BanList] WHERE [AccountID] = @AccountID AND GETDATE() BETWEEN [StartDate] AND [EndDate])
        RETURN 2

    RETURN 0
END